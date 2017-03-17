/* 
Sunstang Main Battery Protection System 2015
July 21 2015, 23:36

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
const float ABSOLUTE_OVER_TEMPERATURE_DISCHARGING = 60; // Celsius
const float ABSOLUTE_OVER_TEMPERATURE_CHARGING = 42; // Celsius


// Battery Pack Status Codes 
const uint8_t STATUS_CODE_SAFE_TO_USE    = 0;
const uint8_t STATUS_CODE_OVERTEMPERATURE_DISCHARGING      = 1;
const uint8_t STATUS_CODE_OVERTEMPERATURE_CHARGING  = 2;
const uint8_t STATUS_CODE_SENSOR_ERROR = 3;


// Battery Events
const uint8_t EVENT_CODE_OVERTEMPERATURE_DISCHARGING        = 10;
const uint8_t EVENT_CODE_OVERTEMPERATURE_CHARGING       = 11;
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
const uint8_t STATUS_PACK_DISCHARGING = 115;
const uint8_t STATUS_PACK_CHARGING = 116;

// Task periods and prescalars
const long    TIMER_PERIOD                      = 1000000;                         // 10Hz
const long    TASK_PERIOD_CheckBattery          = 1000000;     // 10Hz
const long    TASK_PERIOD_UpdateLCD             = 1000000;      // 1Hz
const long    TASK_PERIOD_BroadcastMeasurements = 1000000; // 1Hz
const long    TASK_PERIOD_BroadcastState        = 1000000; // 10Hz
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
const uint8_t DISCHARGE_INDICATOR_PIN = 6; 
const uint8_t ADDR_0 = 6; 
const uint8_t ADDR_1 = 7;
const uint8_t ADDR_2 = 8;
const uint8_t ADDR_3 = 1;
const uint8_t ADDR_4 = 0;
const uint8_t ADDR_5 = 4;
const uint8_t ADDR_6 = 5;


const int canbusInitMaxTries = 5;

const int nSensors = 96;
const unsigned int canbusTemperatureDataStartId = 109;
const uint8_t canbusTemperatureDebugId      = 135;
const uint8_t canbusTemperatureBatteryEventId = 136;
const int switchingDelay = 500000;

const uint16_t LOGIC_LEVEL_THRESH = 246;

byte degreeSymbol[8] = {
    B11100,
    B10100,
    B11100,
    B00000,
    B00000,
    B00000,
    B00000,
  };


/* 
  2. MAIN PROGRAM
*/
LiquidCrystal lcd(A5, A4, A0, A1, A2, A3);
MCP_CAN CAN(10);    
unsigned int sensorReadings[nSensors];
boolean canbusActive = false;

volatile CAR_STATE carState = STATE_IDLE;

volatile boolean newTimeSlice = false;
boolean task_CheckBattery_Ready = false;
boolean task_UpdateLCD_Ready = false;
boolean task_BroadcastMeasurements_Ready = false;
boolean task_BroadcastState_Ready = false;

uint8_t hottestCellAddr = -1;
int hottestCellReading = 1023;
uint8_t batteryStatusCode = -1;


void setup() {
  // Initialize relay pins
  pinMode(RELAY_PIN_MAIN, OUTPUT);
  pinMode(RELAY_PIN_CHARGE, OUTPUT);
  digitalWrite(RELAY_PIN_MAIN,LOW);
  digitalWrite(RELAY_PIN_CHARGE,LOW);
  
  // Init the sensors
  initTemperatureSensor();

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
}
/*
  3. HELPER FUNCTIONS
*/ 

static boolean isOpenCircuited(uint16_t rawValue)
{
  return rawValue >= 1023;
} 

static boolean isClosedCircuited(uint16_t rawValue)
{
  return rawValue <= 0;
} 

static byte checkBatteryPackStatus()
{
  hottestCellAddr = -1;
  hottestCellReading = 1023;
  boolean discharging = isDischarging();
  uint16_t nCellsOverteperature = 0;
  uint16_t nCellsHardwareError = 0;

  if (discharging)
  {
    canbusStatusSend(STATUS_PACK_DISCHARGING);
  }
  else
  {
    canbusStatusSend(STATUS_PACK_CHARGING);
  }

  // Loop through each and every cell
  for (uint8_t currentAddr = 0; currentAddr < nSensors; currentAddr++)
  {
    // Write the address
    writeAddressToPins(currentAddr);

    // Wait some time to allow the MUX to finish switching
    delayMicroseconds(switchingDelay);

    // Read the value on the selected sensor and save to memory
    int rawValue = analogRead(A7);
    sensorReadings[currentAddr] = rawValue;

    // Update the maximum if a more extreme value has been found
    if (rawValue < hottestCellReading)
    {
      hottestCellAddr = currentAddr;
      hottestCellReading = rawValue;
    }

    float logicalTemp = calculateTemperature(rawValue);

    if (discharging && isOvertemperatureDischarging(logicalTemp))
    {
      nCellsOverteperature++;
      canbusEventSend(currentAddr, EVENT_CODE_OVERTEMPERATURE_DISCHARGING);
    }
    else if (!discharging && isOvertemperatureCharging(logicalTemp))
    {
      nCellsOverteperature++;
      canbusEventSend(currentAddr, EVENT_CODE_OVERTEMPERATURE_CHARGING);
    } 
    else if (isOpenCircuited(rawValue))
    {
      nCellsHardwareError++;
      canbusEventSend(currentAddr, EVENT_CODE_SENSOR_ERROR);
    }
    else if (isClosedCircuited(rawValue))
    {
      nCellsHardwareError++;
      canbusEventSend(currentAddr, EVENT_CODE_SENSOR_ERROR);
    }
  }

  if (nCellsOverteperature > 0)
  {
    if (discharging)
    {
      batteryStatusCode = STATUS_CODE_OVERTEMPERATURE_DISCHARGING;
    }
    else
    {
      batteryStatusCode = STATUS_CODE_OVERTEMPERATURE_CHARGING;
    }
  }
  else if (nCellsHardwareError > 0)
  {
    batteryStatusCode = STATUS_CODE_SENSOR_ERROR;
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
    CAN.sendMsgBuf(canbusTemperatureDebugId, 0, 1, &statusCode);  
    delay(2); 
  }
}

void canbusEventSend(uint8_t cellNumber, uint8_t status)
{
  if (canbusActive)
  {
    uint8_t message[2] = {cellNumber, status};
    CAN.sendMsgBuf(canbusTemperatureBatteryEventId, 0, 2, message);
    delay(2);
  }
}

void showMaxTempOnLCD(int addr, float reading)
{
  // Write the maximum temperature and the address of that cell to LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Max temp: ");
  lcd.setCursor(10, 0);
  lcd.print(reading);
  lcd.setCursor(14, 0);
  lcd.write(byte(0));
  lcd.setCursor(15, 0);
  lcd.print("C");
  lcd.setCursor(0, 1);
  lcd.print("on cell ");
  lcd.setCursor(8, 1);
  lcd.print(addr);

}

void writeAddressToPins(unsigned char addressBits)
{

  digitalWrite(ADDR_0, bitRead(addressBits,0));
  digitalWrite(ADDR_1, bitRead(addressBits,1));  
  digitalWrite(ADDR_2, bitRead(addressBits,2));     
  digitalWrite(ADDR_3, bitRead(addressBits,3));
  digitalWrite(ADDR_4, bitRead(addressBits,4));
  digitalWrite(ADDR_5, bitRead(addressBits,5));
  digitalWrite(ADDR_6, bitRead(addressBits,6));
}



void initTemperatureSensor()
{
  pinMode(ADDR_0, OUTPUT);
  pinMode(ADDR_1, OUTPUT);
  pinMode(ADDR_2, OUTPUT);
  pinMode(ADDR_3, OUTPUT);
  pinMode(ADDR_4, OUTPUT);
  pinMode(ADDR_5, OUTPUT);
  pinMode(ADDR_6, OUTPUT);
  analogReference(DEFAULT);
  analogRead(7);
  digitalWrite(ADDR_0, LOW);
  digitalWrite(ADDR_1, LOW);
  digitalWrite(ADDR_2, LOW);
  digitalWrite(ADDR_3, LOW);
  digitalWrite(ADDR_4, LOW);
  digitalWrite(ADDR_5, LOW);
  digitalWrite(ADDR_6, LOW);
}

void initLCD()
{

  lcd.createChar(0, degreeSymbol);

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
  lcd.print("Sunstang BPS2015");
  lcd.setCursor(0,1);
  lcd.print("2015.07.21.2337");
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


boolean isOvertemperatureCharging(float logicalTemp)
{
  return logicalTemp > ABSOLUTE_OVER_TEMPERATURE_CHARGING;
}
boolean isOvertemperatureDischarging(float logicalTemp)
{
  return logicalTemp > ABSOLUTE_OVER_TEMPERATURE_DISCHARGING;
}

boolean isDischarging()
{
  return analogRead(DISCHARGE_INDICATOR_PIN) > LOGIC_LEVEL_THRESH;
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

float calculateTemperature(uint16_t adcVal)
{
  if (adcVal > 550)
  {
    return  adcVal*(-0.051395817)+64.6869847;
  }
  if (adcVal <= 197)
  {
    return  adcVal*(-0.080820328)+73.01123921;
  }
  else
  {
    return  adcVal*(-0.059458674)+68.71374138;
  }
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
      case STATUS_CODE_OVERTEMPERATURE_DISCHARGING:
      case STATUS_CODE_OVERTEMPERATURE_CHARGING:
      case STATUS_CODE_SENSOR_ERROR:
      default:
        carState = STATE_IDLE;
        break;
    }
    
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

  if (currentFrame <= firstFrameLength)
  {
    // Write highest and lowest value to LCD
    float hottestTemp = calculateTemperature(hottestCellReading);
    showMaxTempOnLCD(hottestCellAddr, hottestTemp);
    
  }
  else
  {
    // Display the second screen
    switch (batteryStatusCode)
    {
      case STATUS_CODE_SAFE_TO_USE:
      {
        // Write highest and lowest value to LCD
        float hottestTemp = calculateTemperature(hottestCellReading);
        showMaxTempOnLCD(hottestCellAddr, hottestTemp);
        break;
      }
      case STATUS_CODE_OVERTEMPERATURE_DISCHARGING:
        lcd.setCursor(0, 0);
        lcd.print("Overtemperature ");
        lcd.setCursor(0, 1);
        lcd.print("on discharge    ");
        break;
      case STATUS_CODE_OVERTEMPERATURE_CHARGING:
        lcd.setCursor(0, 0);
        lcd.print("Overtemperature ");
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
    for (int msg = 0; msg < nSensors/4; msg++) // TODO: Make the loop work with any value of nSensors
    {
      unsigned char array[8] = {
        highByte(sensorReadings[4*msg]), 
        lowByte(sensorReadings[4*msg]),
        highByte(sensorReadings[4*msg+1]), 
        lowByte(sensorReadings[4*msg+1]),
        highByte(sensorReadings[4*msg+2]), 
        lowByte(sensorReadings[4*msg+2]),
        highByte(sensorReadings[4*msg+3]), 
        lowByte(sensorReadings[4*msg+3]), 
      };
      unsigned int canbusDataID = canbusTemperatureDataStartId + msg;
      CAN.sendMsgBuf(canbusDataID, 0, 8, array);  
      delay(2);
    }
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

