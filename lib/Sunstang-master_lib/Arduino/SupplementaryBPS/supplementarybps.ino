/* 
Sunstang Supplementary Battery Protection System 2015
June 24 2015 Codefreeze for Friday Unvieling 
*/

#include <SPI.h>
#include <LiquidCrystal.h>
#include <TimerOne.h>
#include <mcp_can.h>


/* 
  1. CONSTANTS AND CLASS DEFINIONS
*/

// Thresholds
const uint8_t ABSOLUTE_OVER_TEMPERATURE = 42; // degrees celcius
const uint8_t ABSOLUTE_OVER_VOLTAGE = 15.6; // volts
const uint8_t ABSOLUTE_UNDER_VOLTAGE = 12; //volts


// Battery Pack Status Codes
const uint8_t STATUS_CODE_TEMPERATURE_SAFE = 0;
const uint8_t STATUS_CODE_VOLTAGE_SAFE = 1;
const uint8_t STATUS_CODE_OVERTEMPERATURE = 2;
const uint8_t STATUS_CODE_OVERVOLTAGE = 3;
const uint8_t STATUS_CODE_UNDERVOLTAGE = 4;
const uint8_t STATUS_CODE_TEMP_SENSOR_ERROR = 5;
const uint8_t STATUS_CODE_VOLTAGE_SENSOR_ERROR = 6;

// Battery Events
const uint8_t EVENT_CODE_OVERTEMPERATURE = 10;
const uint8_t EVENT_CODE_OVERVOLTAGE = 11;
const uint8_t EVENT_CODE_UNDERVOLTAGE = 12;
const uint8_t EVENT_CODE_TEMP_SENSOR_ERROR = 13;
const uint8_t EVENT_CODE_VOLTAGE_SENSOR_ERROR = 14;

// Controller Status Codes
const uint8_t STATUS_PROGRAM_STARTED     = 100;
const uint8_t STATUS_RELAY_ENABLED  = 101;
const uint8_t STATUS_RELAY_DISABLED = 102;
const uint8_t STATUS_START_BUTTON_PRESSED     = 103;
const uint8_t STATUS_TASK_CHECKTEMPERATURE_RUN_ACTIVE     = 104;
const uint8_t STATUS_TASK_CHECKTEMPERATURE_RUN_IDLE     = 105;
const uint8_t STATUS_TASK_CHECKVOLTAGE_RUN_ACTIVE     = 106;
const uint8_t STATUS_TASK_CHECKVOLTAGE_RUN_IDLE     = 107;
const uint8_t STATUS_TASK_UPDATELCD_RUN     = 108;
const uint8_t STATUS_TASK_BROADCASTMEASUREMENTS_RUN    = 109;
const uint8_t STATUS_TASK_TEMPERATURE_BROADCASTSTATE_ACTIVE     = 110;
const uint8_t STATUS_TASK_TEMPERATURE_BROADCASTSTATE_IDLE     = 111;
const uint8_t STATUS_TASK_VOLTAGE_BROADCASTSTATE_ACTIVE         = 112;
const uint8_t STATUS_TASK_VOLTAGE_BROADCASTSTATE_IDLE         = 113;

// Task periods and prescalars
const unsigned long	TIMER_PERIOD = 500000; // Period = 500000us or 0.5s
const unsigned long	TASK_PERIOD_CheckTemperature = 500000; // 1s
const unsigned long     TASK_PERIOD_CheckVoltage = 1000000; // 2s
const unsigned long	TASK_PERIOD_UpdateLCD = 1500000; // 3s
const unsigned long	TASK_PERIOD_BroadcastMeasurements = 1500000; // 3s
const unsigned long     TASK_PERIOD_BroadcastState = 1500000; // 3s

const unsigned long 	TASK_PRESCALAR_CheckTemperature = TASK_PERIOD_CheckTemperature / TIMER_PERIOD;
const unsigned long     TASK_PRESCALAR_CheckVoltage = TASK_PERIOD_CheckVoltage / TIMER_PERIOD;
const unsigned long 	TASK_PRESCALAR_UpdateLCD = TASK_PERIOD_UpdateLCD / TIMER_PERIOD;
const unsigned long 	TASK_PRESCALAR_BroadcastMeasurements = TASK_PERIOD_BroadcastMeasurements / TIMER_PERIOD;
const unsigned long     TASK_PRESCALAR_BroadcastState = TASK_PERIOD_BroadcastState / TIMER_PERIOD;

enum CAR_STATE 
{
  STATE_IDLE,
  STATE_ACTIVE
};

//pinouts
const uint8_t START_BUTTON_PIN = 2;
const uint8_t RELAY_PIN = 9;
const uint8_t ADDR_0 = 8;
const uint8_t ADDR_1 = 7;
const uint8_t ADDR_2 = 6;
const uint8_t ADDR_3 = 5;
const uint8_t ADDR_4 = 4;
const uint8_t ADDR_5 = 3;

const int canbusInitMaxTries = 5;

const int nTempSensors = 16;
const int switchingDelay = 500000;

const unsigned int canbusSBPSDataStartId = 109;
const uint8_t canbusTemperatureDebugId      = 135;
const uint8_t canbusVoltageDebugId          = 136;
const uint8_t canbusTemperatureBatteryEventId = 137;
const uint8_t canbusVoltageBatteryEventId     = 138;


//create degree symbol
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
unsigned int sensorTempReadings[nTempSensors];
unsigned int sensorVoltReading;
boolean canbusActive = false;

volatile CAR_STATE carState = STATE_ACTIVE;

volatile boolean newTimeSlice = false;
boolean task_CheckTemperature_Ready = false;
boolean task_CheckVoltage_Ready = false;
boolean task_UpdateLCD_Ready = false;
boolean task_BroadcastMeasurements_Ready = false;
boolean task_BroadcastState_Ready = false;

uint8_t hottestAddr = -1;
int hottestReading = 1023;
int voltageReading = 1023;
uint8_t temperatureStatusCode = -1;
uint8_t voltageStatusCode = -1;

void setup() {
  
  // Initialize relay pin
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  
  //Initialize temp sensors
  initTempSens();
  
  //Init LCD
  initLCD();
  
  // Initialize timer1, used for implementing time-slice cyclic task scheduling
  Timer1.initialize(TIMER_PERIOD);  
  Timer1.attachInterrupt(timerISR); 

  // Initialize the external interrupt, used to put the car into the active state
  attachInterrupt(0, startCarFunction, FALLING);

  // Init Canbus
  //initCanbus();
}


void loop() {

  waitForTimeSlice();

  // task_CheckTemperature checks the temperature of the batteries and control the relay
  if (task_CheckTemperature_Ready)
  {
    task_CheckTemperature();
    task_CheckTemperature_Ready = false;
  }
  
  // task_CheckVoltage check the voltages of the batteries and controls the relay
  if (task_CheckVoltage_Ready)
  {
    task_CheckVoltage();
    task_CheckVoltage_Ready = false;
  }
  
  // task_UpdateLCD writes to the LCD screen the minimum/maximum voltages (and the status of the controller if not running)
  if (task_UpdateLCD_Ready)
  {
    task_UpdateLCD();
    task_UpdateLCD_Ready = false;
  }

  // task_BroadcastMeasurements broadcasts the battery voltages on the canbus
  if (task_BroadcastMeasurements_Ready)
  {
    //task_BroadcastMeasurements();
    //task_BroadcastMeasurements_Ready = false;
  }
}

/*
  3. HELPER FUNCTIONS
*/ 

void displayMaximum(float temp, float voltage, int addr)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("MaxT: ");
  lcd.setCursor(6, 0);
  lcd.print(temp);
  //lcd.setCursor(8,0);
  //lcd.write(byte(0));
  //lcd.setCursor(9,0);
  //lcd.print("C");
  //lcd.setCursor(12,1);
  //lcd.print("on ");
  lcd.setCursor(13,0);
  lcd.print("#");
  lcd.setCursor(14,0);
  lcd.print(addr);
  lcd.setCursor(0,1);
  lcd.print("Voltage: ");
  lcd.setCursor(9,1);
  lcd.print(voltage);
}

void writeAddressToPins(unsigned char addressBits)
{
  digitalWrite(ADDR_0, bitRead(addressBits,0));
  digitalWrite(ADDR_1, bitRead(addressBits,1));  
  digitalWrite(ADDR_2, bitRead(addressBits,2));     
  digitalWrite(ADDR_3, bitRead(addressBits,3));
  digitalWrite(ADDR_4, bitRead(addressBits,4));
  digitalWrite(ADDR_5, bitRead(addressBits,5));
}

void initTempSens()
{  
  pinMode(ADDR_0, OUTPUT);
  pinMode(ADDR_1, OUTPUT);
  pinMode(ADDR_2, OUTPUT);
  pinMode(ADDR_3, OUTPUT);
  pinMode(ADDR_4, OUTPUT);
  pinMode(ADDR_5, OUTPUT);
  analogReference(INTERNAL);
  digitalWrite(ADDR_0, LOW);
  digitalWrite(ADDR_1, LOW);
  digitalWrite(ADDR_2, LOW);
  digitalWrite(ADDR_3, LOW);
  digitalWrite(ADDR_4, LOW);
  digitalWrite(ADDR_5, LOW);
  analogRead(A7);
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
  lcd.print("Sunstang");
  lcd.setCursor(0,1);
  lcd.print("2015");
}

void startCarFunction()
{
  // Start button pressed
  // This function is the only way to enter status STATE_ACTIVE
  carState = STATE_ACTIVE;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("active");
  delay(10000);
}
/*
void initCanbus()
{
  canbusActive = false;

  for (int i = 0; i < canbusInitMaxTries && canbusActive == false; i++)
  {
    if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
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
*/

static boolean isOpenCircuited(uint16_t rawValue)
{
  return rawValue >= 1023;
} 

static boolean isClosedCircuited(uint16_t rawValue)
{
  return rawValue <= 0;
} 

boolean isVoltageSensorError(uint8_t rawValue)
{
  return rawValue <= 0;
}

boolean isOvertemperature(float logicalTemp)
{
  return logicalTemp > ABSOLUTE_OVER_TEMPERATURE;
}

boolean isOvervoltage(uint8_t logicalVoltage)
{
  return logicalVoltage > ABSOLUTE_OVER_VOLTAGE;
}

boolean isUndervoltage(uint8_t logicalVoltage)
{
  return logicalVoltage < ABSOLUTE_UNDER_VOLTAGE;
}

void enableRelay()
{
  if (digitalRead(RELAY_PIN != HIGH))
  {
    //canbusStatusSend(STATUS_RELAY_ENABLED);
    digitalWrite(RELAY_PIN, HIGH);
  }
}

void disableRelay()
{
  if (digitalRead(RELAY_PIN !=LOW))
  {
    //canbusStatusSend(STATUS_RELAY_DISABLED);
    digitalWrite(RELAY_PIN, LOW);
  }
}

static byte checkTemperatureStatus()
{
  hottestAddr = -1;
  hottestReading = 1023;
  uint16_t nCellsOverteperature = 0;
  uint16_t nCellsHardwareError = 0;
  
  //loop through every cell
  for(int currentAddr = 0; currentAddr < 16; currentAddr++)
  {
   //write the address
   writeAddressToPins(currentAddr);
   delayMicroseconds(switchingDelay);
   
   // Read value on selected sensor and save to memory
   int rawValue = analogRead(A7);
   sensorTempReadings[currentAddr] = rawValue;
   
   // Update the maximum if a more extreme value found
   if(rawValue < hottestReading)
   {
     hottestAddr = currentAddr;
     hottestReading = rawValue;
   }
   
   float logicalTemp = calculateTemperature(rawValue);
   
   if (isOpenCircuited(rawValue))
   {
     nCellsHardwareError++;
     //canbusTemperatureEventSend(currentAddr, EVENT_CODE_TEMP_SENSOR_ERROR);
   }
   else if (isClosedCircuited(rawValue))
   {
     nCellsHardwareError++;
     //canbusTemperatureEventSend(currentAddr, EVENT_CODE_TEMP_SENSOR_ERROR);
   }
   else if (isOvertemperature(logicalTemp))  //WHAT IF IDENTIFIED AS OVERTEMP BUT IS HARDWARE ERROR???
   {
     nCellsOverteperature++;
     //canbusTemperatureEventSend(currentAddr, EVENT_CODE_OVERTEMPERATURE);
   }

  
  if (nCellsOverteperature > 0)
  {
     temperatureStatusCode = STATUS_CODE_OVERTEMPERATURE;
  }
  else if (nCellsHardwareError > 0)
  {
    temperatureStatusCode = STATUS_CODE_TEMP_SENSOR_ERROR;
  }
  else
  {
    temperatureStatusCode = STATUS_CODE_TEMPERATURE_SAFE;
  }
 }
  return temperatureStatusCode;
  
}

 
 static byte checkVoltageStatus()
 {
   uint16_t overvoltage = 0;
   uint16_t undervoltage = 0;
   uint16_t voltage_error = 0; 
   
   int rawValue = analogRead(A6);
   voltageReading = rawValue;
   float logicalVoltage = calculateVoltage(rawValue);
   
   if(isVoltageSensorError(rawValue))
   {
     //canbusVoltageEventSend(EVENT_CODE_VOLTAGE_SENSOR_ERROR);
     voltage_error++;
   }  
   else if(isOvervoltage(logicalVoltage))
   {
     //canbusVoltageEventSend(EVENT_CODE_OVERVOLTAGE);
     overvoltage++;
   }
   else if(isUndervoltage(logicalVoltage))
   {
     //canbusVoltageEventSend(EVENT_CODE_UNDERVOLTAGE);
     undervoltage++;
   }


  if (overvoltage)
  {
     voltageStatusCode = STATUS_CODE_OVERVOLTAGE;
  }
  else if (undervoltage > 0)
  {
    voltageStatusCode = STATUS_CODE_UNDERVOLTAGE;
  }
  else if (voltage_error > 0)
  {
    voltageStatusCode = STATUS_CODE_VOLTAGE_SENSOR_ERROR;
  }
  else
  {
    voltageStatusCode = STATUS_CODE_VOLTAGE_SAFE;
  }
  
  return voltageStatusCode;

}

/*
void canbusTemperatureStatusSend(uint8_t statusCode)
{
  if (canbusActive)
  {
    CAN.sendMsgBuf(canbusTemperatureDebugId, 0, 1, &statusCode);  
    delay(2); 
  }
}

void canbusTemperatureEventSend(uint8_t cellNumber, uint8_t status)
{
  if (canbusActive)
  {
    uint8_t message[2] = {cellNumber, status};
    CAN.sendMsgBuf(canbusTemperatureBatteryEventId, 0, 2, message);
    delay(2);
  }
}

void canbusVoltageStatusSend(uint8_t statusCode)
{
  if (canbusActive)
  {
    CAN.sendMsgBuf(canbusVoltageDebugId, 0, 1, &statusCode);  
    delay(2); 
  }
}

void canbusVoltageEventSend(uint8_t status)
{
  if (canbusActive)
  {
    uint8_t message[1] = {status};
    CAN.sendMsgBuf(canbusVoltageBatteryEventId, 0, 1, message);
    delay(2);
  }
}
*/

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

float calculateVoltage(uint16_t adcVal)
{
  return   adcVal*(0.010228166)+8.58;
}

/*
  4. TASK SCHEDULING CODE
*/

// The following two functions implement time-slice cyclic scheduling

// This function is called automatically every 'TIMER_PERIOD' microseconds
// Note the real-time nature of this type of programming; we are not relying on delays or specific function execution times.
// Instead, we rely on a free-running timer (independant of our program) to trigger certain functions at EXACT intervals.
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
  static unsigned char checkTemperatureCount = 0;
  static unsigned char checkVoltageCount = 0;
  static unsigned char updateLcdCount = 0;
  static unsigned char broadcastMeasurementsCount = 0;

  // Determine if any of the tasks is ready to run using prescalar value
  
  if (checkTemperatureCount >= TASK_PRESCALAR_CheckTemperature)
  {
    task_CheckTemperature_Ready = true;
    checkTemperatureCount = 0;
  }
  
  if (checkVoltageCount >= TASK_PRESCALAR_CheckVoltage)
  {
    task_CheckVoltage_Ready = true;
    checkVoltageCount = 0;
  }
  
  if (updateLcdCount >= TASK_PRESCALAR_UpdateLCD)
  {
    task_UpdateLCD_Ready = true;
    updateLcdCount = 0;
  }
  
  if (broadcastMeasurementsCount >= TASK_PRESCALAR_BroadcastMeasurements)
  {
    task_BroadcastMeasurements_Ready = true;
    broadcastMeasurementsCount = 0;
  }
  
  // For each task, update count used for prescaling 
  checkTemperatureCount++;
  checkVoltageCount++;
  updateLcdCount++;
  broadcastMeasurementsCount++;
}

/* 
  5. TASK DEFINITIONS 
*/

//temperature monitor routine
void task_CheckTemperature()
{
  if (carState == STATE_ACTIVE)
  {
    //canbusTemperatureStatusSend(STATUS_TASK_CHECKTEMPERATURE_RUN_ACTIVE);

    byte batteryStatusCode = checkTemperatureStatus();

    // Perform check and relay control
    switch (batteryStatusCode)
    {
      case STATUS_CODE_TEMPERATURE_SAFE:
        enableRelay();
        break;
      case STATUS_CODE_OVERTEMPERATURE:
      case STATUS_CODE_TEMP_SENSOR_ERROR:
      default:
        carState = STATE_ACTIVE;  //should be STATE_IDLE
        break;
    }
  }
  else
  {
    // IDLE state: Deactivate all functions and do nothing
    //canbusTemperatureStatusSend(STATUS_TASK_CHECKTEMPERATURE_RUN_IDLE);
    disableRelay();
  }
}

//voltage monitor routine
void task_CheckVoltage()
{
  if (carState == STATE_ACTIVE)
  {
    //canbusVoltageStatusSend(STATUS_TASK_CHECKVOLTAGE_RUN_ACTIVE);

    byte batteryStatusCode = checkVoltageStatus();

    // Perform check and relay control
    switch (batteryStatusCode)
    {
      case STATUS_CODE_VOLTAGE_SAFE:
        enableRelay();
        break;
      case STATUS_CODE_OVERVOLTAGE:
      case STATUS_CODE_UNDERVOLTAGE:
      case STATUS_CODE_VOLTAGE_SENSOR_ERROR:
      default:
        carState = STATE_ACTIVE;   //should be STATE_IDLE
        break;
    }
  }
  
  else
  {
    // IDLE state: Deactivate all functions and do nothing
    //canbusVoltageStatusSend(STATUS_TASK_CHECKVOLTAGE_RUN_IDLE);
    disableRelay();
  }
}

void task_UpdateLCD()
{
  // Blink between two screens
  static uint8_t currentFrame = 1;
  const uint8_t firstFrameLength = 2;
  const uint8_t secondFrameLength = 2;

    // Write highest and lowest value to LCD
    float hottestTemp = calculateTemperature(hottestReading);
    float packVoltage = calculateVoltage(voltageReading);
    displayMaximum(hottestTemp, packVoltage, hottestAddr); 
}

/*
void task_BroadcastTemperatureState()
{
  if (carState == STATE_ACTIVE)
  {
    canbusTemperatureStatusSend(STATUS_TASK_TEMPERATURE_BROADCASTSTATE_ACTIVE);
  }
  else
  {
    canbusTemperatureStatusSend(STATUS_TASK_TEMPERATURE_BROADCASTSTATE_IDLE);
  }
}

void task_BroadcastVoltageState()
{
  if (carState == STATE_ACTIVE)
  {
    canbusVoltageStatusSend(STATUS_TASK_VOLTAGE_BROADCASTSTATE_ACTIVE);
  }
  else
  {
    canbusVoltageStatusSend(STATUS_TASK_VOLTAGE_BROADCASTSTATE_IDLE);
  }
}
*/
