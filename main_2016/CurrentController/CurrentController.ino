/* 
Sunstang Main Battery Protection System 2015
July 14, 22:35

Note: All the code is kept in a single file to simplifiy deployment to hardware.

This controller successfully performs four tasks:
1. Measures temperature of 96 cells, and decides whether or not to enable the main relay (not the charge relay).
   If at any point one of the thermistors become disconnected or shorted, the canbus error code will indicate a hardware error.
2. Update LCD screen with maximum temperature and their corresponding IDs.
3. Brodcast on the canbusall the measured temperatures
4. Broadcast on the canbus the state of the controller


TODO: 
1. Fix charge relay control

The code is organized using the following headings
  1. Constants and Class Definitions
  2. Main Program
  3. Helper Functions
  4. Task Scheduling Code
  5. Task Definitions
*/

#include <SPI.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <mcp_can.h>

/* 
  1. CONSTANTS AND CLASS DEFINIONS
*/

// Thresholds
const float ABSOLUTE_OVER_CURRENT_DISCHARGING = -130; // A
const float ABSOLUTE_OVER_CURRENT_CHARGING = 15; // A


// Battery Pack Status Codes 
const uint8_t STATUS_CODE_SAFE_TO_USE    = 0;
const uint8_t STATUS_CODE_OVERCURRENT_DISCHARGING      = 1;
const uint8_t STATUS_CODE_OVERCURRENT_CHARGING  = 2;
const uint8_t STATUS_CODE_SENSOR_ERROR = 3;


// Battery Events
const uint8_t EVENT_CODE_OVERCURRENT_DISCHARGING        = 10;
const uint8_t EVENT_CODE_OVERCURRENT_CHARGING       = 11;
const uint8_t EVENT_CODE_SENSOR_ERROR = 12;


// Controller Status Codes
const uint8_t STATUS_PROGRAM_STARTED     = 100;
const uint8_t STATUS_MAIN_RELAY_ENABLED  = 101;
const uint8_t STATUS_MAIN_RELAY_DISABLED = 102;
const uint8_t STATUS_CHARGE_RELAY_ENABLED     = 103;
const uint8_t STATUS_CHARGE_RELAY_DISABLED     = 104;
const uint8_t STATUS_START_BUTTON_PRESSED     = 105;
const uint8_t STATUS_TASK_CHECKBATTERY_RUN_ACTIVE     = 106;
const uint8_t STATUS_TASK_CHECKBATTERY_RUN_IDLE     = 107;
const uint8_t STATUS_TASK_UPDATELCD_RUN     = 108;
const uint8_t STATUS_TASK_BROADCASTMEASUREMENTS_RUN    = 109;
const uint8_t STATUS_TASK_BROADCASTSTATE_ACTIVE     = 110;
const uint8_t STATUS_TASK_BROADCASTSTATE_IDLE     = 111;

// Task periods and prescalars
const long    TIMER_PERIOD                      = 10000;     // 100Hz                    
const long    TASK_PERIOD_CheckBattery          = 10000;     // 100Hz
const long    TASK_PERIOD_UpdateLCD             = 1000000;      // 1Hz
const long    TASK_PERIOD_BroadcastMeasurements = 1000000; // 1Hz
const long    TASK_PERIOD_BroadcastState        = 1000000; // 1Hz
const long    TASK_PRESCALAR_CheckBattery             = TASK_PERIOD_CheckBattery / TIMER_PERIOD;
const long    TASK_PRESCALAR_UpdateLCD                = TASK_PERIOD_UpdateLCD / TIMER_PERIOD;
const long    TASK_PRESCALAR_BroadcastMeasurements    = TASK_PERIOD_BroadcastMeasurements / TIMER_PERIOD;
const long    TASK_PRESCALAR_BroadcastState           = TASK_PERIOD_BroadcastState / TIMER_PERIOD;

enum CAR_STATE 
{
  STATE_IDLE,
  STATE_ACTIVE
};

// Pin-outs
const uint8_t     START_BUTTON_PIN = 2;
const uint8_t     RELAY_PIN_MAIN = 9;
const uint8_t     RELAY_PIN_CHARGE = 3;
const uint8_t DISCHARGE_OUTPUT_INDICATOR_PIN = 4; 


const int canbusInitMaxTries = 5;
const unsigned int canbusCurrentDataStartId = 108;
const uint8_t canbusCurrentDebugId      = 137;
const uint8_t canbusCurrentBatteryEventId = 138;

/* 
  2. MAIN PROGRAM
*/
LiquidCrystal lcd(A5, A4, A0, A1, A2, A3);
MCP_CAN CAN(10);    
boolean canbusActive = false;

volatile CAR_STATE carState = STATE_IDLE; 

volatile boolean newTimeSlice = false;
boolean task_CheckBattery_Ready = false;
boolean task_UpdateLCD_Ready = false;
boolean task_BroadcastMeasurements_Ready = false;
boolean task_BroadcastState_Ready = false;

uint8_t batteryStatusCode = -1;
const uint16_t chargingDischargingThreshold = 836; // -0.06A
uint16_t maxReading = chargingDischargingThreshold;
uint16_t minReading = chargingDischargingThreshold;
float offset;

// Variables for calculating running average
const uint16_t samplesConsideredInAverage = 100;
uint16_t recentSampleValues[samplesConsideredInAverage];
float currentRunningAverage = chargingDischargingThreshold;
uint16_t oldestIndex = 0;

void setup() {

  Serial.begin(115200);

  // Initialize relay pins
  pinMode(RELAY_PIN_MAIN, OUTPUT);
  pinMode(RELAY_PIN_CHARGE, OUTPUT);
  digitalWrite(RELAY_PIN_MAIN,LOW);
  digitalWrite(RELAY_PIN_CHARGE,LOW);
  pinMode(DISCHARGE_OUTPUT_INDICATOR_PIN, OUTPUT);
  
  // Init the sensors
  initCurrentSensor();

  // Init LCD
  initLCD();

  // Initialize timer1, used for implementing time-slice cyclic task scheduling
  Timer1.initialize(TIMER_PERIOD);  
  Timer1.attachInterrupt(timerISR); 

  // Initialize the external interrupt, used to put the car into the active state
  attachInterrupt(0, startCarFunction, FALLING);

  // Init Canbus
  initCanbus();

}

// Use time-slice scheduler
// At the beginning of each time-slice, the scheduler waits for the timer event, then tasks are given an opportunity to run, if ready
void loop() {

  waitForTimeSlice();

  // task_CheckBattery checks the voltages of batteries and controls both relays accordingly
  if (task_CheckBattery_Ready)
  {
    task_CheckBattery();
    task_CheckBattery_Ready = false;
  }

  // task_UpdateLCD_Ready writes to the LCD screen the minimum/maximum voltages (and the status of the controller if not running)
  if (task_UpdateLCD_Ready)
  {
    task_UpdateLCD();
    task_UpdateLCD_Ready = false;
  }

  // task_BroadcastMeasurements broadcasts the battery voltages on the canbus
  if (task_BroadcastMeasurements_Ready)
  {
    task_BroadcastMeasurements();
    task_BroadcastMeasurements_Ready = false;
  }

  if (task_BroadcastState_Ready)
  {
    task_BroadcastState();
    task_BroadcastState_Ready = false;
  }
  //Serial.println();
}
/*
  3. HELPER FUNCTIONS
*/ 
boolean hasHardwareError(uint16_t rawValue)
{
  return rawValue == 0;
}
boolean isOvercurrentDischarging(float current)
{
  return current < ABSOLUTE_OVER_CURRENT_DISCHARGING;
}
boolean isOvercurrentCharging(float current)
{
  return current > ABSOLUTE_OVER_CURRENT_CHARGING;
}

boolean updateDischargingPin()
{
  digitalWrite(DISCHARGE_OUTPUT_INDICATOR_PIN, isDischarging());
}

uint16_t getMostRecentInstantaneousReading()
{
  uint16_t newestIndex = oldestIndex - 1;

  if (newestIndex < 0)
  {
    newestIndex += samplesConsideredInAverage;
  }

  return recentSampleValues[newestIndex];
}

static byte checkBatteryPackStatus()
{
  boolean discharging = isDischarging();

  // Read the value on the sensor and save to memory
  int newValue = analogRead(A7);

  //Serial.print("NewValue: ");
  //Serial.println(newValue);

  calculateNextAverage(newValue);

  //Serial.print("Now Average: ");
  //Serial.println(currentRunningAverage);

  // Update peak values
  minReading = min(minReading, currentRunningAverage);
  maxReading = max(maxReading, currentRunningAverage);
  Serial.println("Min");
  Serial.print(minReading);
  Serial.println("Max");
  Serial.print(maxReading);

  float logicalCurrent = calculateCurrent(currentRunningAverage);
  //Serial.print("\tCurrent: ");
  //Serial.print(logicalCurrent);
  if (hasHardwareError(newValue))
  {
    canbusEventSend(EVENT_CODE_SENSOR_ERROR);
    batteryStatusCode = STATUS_CODE_SENSOR_ERROR;
  }
  else if (isOvercurrentCharging(logicalCurrent))
  {

    canbusEventSend(EVENT_CODE_OVERCURRENT_CHARGING);
    batteryStatusCode = STATUS_CODE_OVERCURRENT_CHARGING;
  }
  else if (isOvercurrentDischarging(logicalCurrent))
  {
    canbusEventSend(EVENT_CODE_OVERCURRENT_DISCHARGING);
    batteryStatusCode = STATUS_CODE_OVERCURRENT_DISCHARGING;
  }
  else
  {
    batteryStatusCode = STATUS_CODE_SAFE_TO_USE;
  }

  return batteryStatusCode;
}

void canbusStatusSend(uint8_t statusCode)
{
  if (canbusActive)
  {
    CAN.sendMsgBuf(canbusCurrentDebugId, 0, 1, &statusCode);  
    delay(2); 
  }
}

void canbusEventSend(uint8_t status)
{
  if (canbusActive)
  {
    CAN.sendMsgBuf(canbusCurrentBatteryEventId, 0, 1, &status);
    delay(2);
  }
}

void showCurrentOnLCD(float current, float peakCharge, float peakDischarge)
{
  // Write the maximum temperature and the address of that cell to LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(current);
  lcd.setCursor(6, 0);
  lcd.print(" A            ");
  lcd.setCursor(0, 1);
  lcd.print("     Ai -     Ao");
  lcd.setCursor(0, 1);
  lcd.print(peakCharge);
  lcd.setCursor(9, 1);
  lcd.print(peakDischarge);
}



void initCurrentSensor()
{
  // Change the reference voltage to 1.1V
  analogReference(INTERNAL);

  // A few calls to analogRead must be made for the new reference voltage to appear at the Vref pin
  for (int i = 0; i < 10; i++)
  {
    analogRead(7);
  }

  Serial.println("Reading in the first set of values");
  // Populate the first few raw values inside sampleArray and calculate average
  float sum = 0;
  for (int i = 0; i < samplesConsideredInAverage; i++)
  {
    recentSampleValues[i] = analogRead(7);
    Serial.println(recentSampleValues[i]);
    sum += recentSampleValues[i];
  }
  Serial.println("Sum:");
  Serial.println(sum);
  currentRunningAverage = sum / samplesConsideredInAverage;
  Serial.println(currentRunningAverage);
  offset = chargingDischargingThreshold - currentRunningAverage;
}

void calculateNextAverage(uint16_t newValue)
{
  Serial.print("\nInput: ");
  Serial.println(newValue);
  Serial.print("oldest index: ");
  Serial.println(oldestIndex);

  Serial.print("recentSampleValues[OldestIndex]: ");
  Serial.println(recentSampleValues[oldestIndex]);


  float difference = ((int16_t)newValue-(int16_t)recentSampleValues[oldestIndex]);
  Serial.print("diff: ");
  Serial.println(difference);
  float scaledDiff = difference/samplesConsideredInAverage;
  Serial.print("scaled diff: ");
  Serial.println(scaledDiff);
  currentRunningAverage += scaledDiff;

  Serial.print("output: ");
  Serial.println(currentRunningAverage);

  // Replace oldest value with new value
  recentSampleValues[oldestIndex] = newValue;
  
  // Increment oldestindex
  oldestIndex++;
  oldestIndex = oldestIndex % samplesConsideredInAverage;
}

void initLCD()
{
  // Init the LCD
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  lcd.begin(16, 2);

  // Print a message to the LCD.
  lcd.setCursor(0,0);
  lcd.print("Sunstang");
  lcd.setCursor(0,1);
  lcd.print("2015");
}

void startCarFunction()
{
  // Start button pressed
  // This function is the only way to enter status STATE_ACTIVE
  carState = STATE_ACTIVE;
}

void initCanbus()
{
  canbusActive = false;

  for (int i = 0; i < canbusInitMaxTries && canbusActive == false; i++)
  {
    if(CAN_OK == CAN.begin(CAN_1000KBPS))                  
    {
    //  Serial.println("CAN BUS Shield init ok!");
      canbusActive = true;
    }
    else
    { 
     // Serial.println("CAN BUS Shield init fail");
     // Serial.println("Init CAN BUS Shield again");
    }
    delay(1000);
  }
}

boolean isDischarging()
{
  return currentRunningAverage < chargingDischargingThreshold;
}

void enableMainRelay()
{
  if (digitalRead(RELAY_PIN_MAIN) != HIGH) 
  {
    canbusStatusSend(STATUS_MAIN_RELAY_ENABLED);
    digitalWrite(RELAY_PIN_MAIN, HIGH);
  }
}

void disableMainRelay()
{
  if (digitalRead(RELAY_PIN_MAIN) != LOW) 
  {
    canbusStatusSend(STATUS_MAIN_RELAY_DISABLED);
    digitalWrite(RELAY_PIN_MAIN, LOW);
  }
}

void enableChargeRelay()
{
  if (digitalRead(RELAY_PIN_CHARGE) != HIGH) 
  {
    canbusStatusSend(STATUS_CHARGE_RELAY_ENABLED);
    digitalWrite(RELAY_PIN_CHARGE, HIGH);
  }
}

void disableChargeRelay()
{
  if (digitalRead(RELAY_PIN_CHARGE) != LOW) 
  {
    canbusStatusSend(STATUS_CHARGE_RELAY_DISABLED);
    digitalWrite(RELAY_PIN_CHARGE, LOW);
  }
}

float calculateCurrent(uint16_t adcVal)
{
  //float Vo = adcVal * 1.1 / 1023;
  //float Vi = (Vo + 0.5885)/0.3703;
  //float I = (Vi - 4.096)/0.0162;
  adcVal += offset;
  Serial.print("Result after offset = ");
  Serial.println(adcVal);
  return ((33.51459+149.64)/1024)*adcVal -149.64;
}

/* 
  4. TASK SCHEDULING CODE
*/
// The following two functions implement time-slice cyclic scheduling

// timerISR() notifies waitForTimeslice() that a new timeslice arrived
// waitForTimeSlice() halts program execution until it gets the signal from timerISR()
void timerISR()
{
  // Raise timeslice flag
  newTimeSlice = true;
}
void waitForTimeSlice()
{
  // poll timeslice flag. Reset flag upon exit
  while (newTimeSlice == false);
  newTimeSlice = false;

  // At the very beginning of program runtime, initialize count variables used for prescaling
  static unsigned char checkBatteryCount = 0;
  static unsigned char updateLcdCount = 0;
  static unsigned char broadcastCanbusCount = 0;
  static unsigned char broadcastControllerStateCount = 0;

  // Determine if any of the tasks is ready to run using prescalar value
  
  if (checkBatteryCount >= TASK_PRESCALAR_CheckBattery)
  {
    task_CheckBattery_Ready = true;
    checkBatteryCount = 0;
  }
  
  if (updateLcdCount >= TASK_PRESCALAR_UpdateLCD)
  {
    task_UpdateLCD_Ready = true;
    updateLcdCount = 0;
  }
  
  if (broadcastCanbusCount >= TASK_PRESCALAR_BroadcastMeasurements)
  {
    task_BroadcastMeasurements_Ready = true;
    broadcastCanbusCount = 0;
  }

  if (broadcastControllerStateCount >= TASK_PRESCALAR_BroadcastState)
  {
    task_BroadcastState_Ready = true; 
    broadcastControllerStateCount = 0;
  }
  
  // For each task, update count used for prescaling 
  checkBatteryCount++;
  updateLcdCount++;
  broadcastCanbusCount++;
  broadcastControllerStateCount++;
}

/* 
  5. TASK DEFINITIONS 
*/

void task_CheckBattery()
{
  if (carState == STATE_ACTIVE)
  {
    canbusStatusSend(STATUS_TASK_CHECKBATTERY_RUN_ACTIVE);

    byte batteryStatusCode = checkBatteryPackStatus();

    // Perform check and relay control
    switch (batteryStatusCode)
    {
      case STATUS_CODE_SAFE_TO_USE:
        enableMainRelay();
        enableChargeRelay();
        break;
      case STATUS_CODE_OVERCURRENT_DISCHARGING:
      case STATUS_CODE_OVERCURRENT_CHARGING:
      case STATUS_CODE_SENSOR_ERROR:
      default:
        carState = STATE_IDLE;
        break;
    }
    
    // Update the discharging pin
    updateDischargingPin();
  }
  else
  {
    // IDLE state: Deactivate all functions and do nothing
    canbusStatusSend(STATUS_TASK_CHECKBATTERY_RUN_IDLE);
    disableMainRelay();
    disableChargeRelay();
  }
}

void task_UpdateLCD()
{
  // Blink between two screens
  static uint8_t currentFrame = 1;
  const uint8_t firstFrameLength = 2;
  const uint8_t secondFrameLength = 2;
  
  float instantaneousCurrent = calculateCurrent(currentRunningAverage);
  //float peakChargeCurrent = calculateCurrent(maxReading);
  //float peakDischargeCurrent = calculateCurrent(minReading);

  if (currentFrame <= firstFrameLength)
  {
    // Write highest and lowest value to LCD
    showCurrentOnLCD(instantaneousCurrent, 0,0); //peakChargeCurrent, peakDischargeCurrent);
    
  }
  else
  {
    // Display the second screen
    switch (batteryStatusCode)
    {
      case STATUS_CODE_SAFE_TO_USE:
      {
        // Write highest and lowest value to LCD
        showCurrentOnLCD(instantaneousCurrent, 0,0); //peakChargeCurrent, peakDischargeCurrent);
        break;
      }
      case STATUS_CODE_OVERCURRENT_DISCHARGING:
        lcd.setCursor(0, 0);
        lcd.print("Overcurrent ");
        lcd.setCursor(0, 1);
        lcd.print("on discharge    ");
        break;
      case STATUS_CODE_OVERCURRENT_CHARGING:
        lcd.setCursor(0, 0);
        lcd.print("Overcurrent ");
        lcd.setCursor(0, 1);
        lcd.print("on charge       ");
        break;
      case STATUS_CODE_SENSOR_ERROR:
        lcd.setCursor(0, 0);
        lcd.print("Sensor Error    ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        break;
      default:
        break;
    }
  }

  if (currentFrame >= firstFrameLength + secondFrameLength)
  {
    currentFrame = 0;
  }

  currentFrame++;
}

void task_BroadcastMeasurements()
{  
  if (canbusActive)
  {
    unsigned char array[6] = {
      highByte(getMostRecentInstantaneousReading()), 
      lowByte(getMostRecentInstantaneousReading()),
      0x00,
      0x00,
      0x00,
      0x00
    };
    CAN.sendMsgBuf(canbusCurrentDataStartId, 0, 4, array);  
    delay(2);
    
  }
}

void task_BroadcastState()
{
  if (carState == STATE_ACTIVE)
  {
    canbusStatusSend(STATUS_TASK_BROADCASTSTATE_ACTIVE);
  }
  else
  {
    canbusStatusSend(STATUS_TASK_BROADCASTSTATE_IDLE);
  }
}

