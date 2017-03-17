/* 
Sunstang Main Battery Protection System 2015
May 22 2015, 21:25

Note: All the code is kept in a single file to simplifiy deployment to hardware.

This controller successfully performs four tasks:
1. Measures voltages of 32 cells in Sunstang 2015 configuration, and decides whether or not to enable the main/charge relays.
2. Update LCD screen with minimum and maximum voltages and their corresponding IDs.
3. Brodcast on the canbusall the measured voltages
4. Broadcast on the canbus the state of the controller

TODO: 
1. Discharge Control
2. Figure out first-powerup condition for sensors
3. Display on the screen cause of battery event

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
const float   MINIMUM_FULL_VOLTAGE         = 4.00;
const float   ABSOLUTE_OVER_VOLTAGE        = 4.15;
const float   ABSOLUTE_UNDER_VOLTAGE       = 2.85;          

// Battery Pack Status Codes 
enum STATUS_CODE
{
  STATUS_CODE_NEEDS_CHARGING    = 0,
  STATUS_CODE_FULL              = 1,
  STATUS_CODE_OVERVOLTAGE       = 2,
  STATUS_CODE_UNDERVOLTAGE      = 3,
  STATUS_CODE_UNDEROVERVOLTAGE  = 4
};

// Battery Events
enum BatteryEvent
{
  EVENT_CODE_OVERVOLTAGE        = 10,
  EVENT_CODE_UNDERVOLTAGE       = 11
};

// Controller Status Codes
enum ControllerStatus
{
  STATUS_PROGRAM_STARTED     = 100,
  STATUS_MAIN_RELAY_ENABLED  = 101,
  STATUS_MAIN_RELAY_DISABLED = 102,
  STATUS_CHARGE_RELAY_ENABLED     = 103,
  STATUS_CHARGE_RELAY_DISABLED     = 104,
  STATUS_START_BUTTON_PRESSED     = 105,
  STATUS_TASK_CHECKBATTERY_RUN_ACTIVE     = 106,
  STATUS_TASK_CHECKBATTERY_RUN_IDLE     = 107,
  STATUS_TASK_UPDATELCD_RUN     = 108,
  STATUS_TASK_BROADCASTMEASUREMENTS_RUN    = 109,
  STATUS_TASK_BROADCASTSTATE_ACTIVE     = 110,
  STATUS_TASK_BROADCASTSTATE_IDLE     = 111,
  STATUS_FUNCTION_RUN_resetLocalRegisters = 120,
  STATUS_FUNCTION_RUN_initVoltageSensorSPI  = 121,
  STATUS_FUNCTION_RUN_writeConfigurationRegisterGroup = 122,
  STATUS_FUNCTION_RUN_readConfigurationRegisterGroup  = 123,
  STATUS_FUNCTION_RUN_readCellVoltageRegisterGroupA  = 124,
  STATUS_FUNCTION_RUN_readCellVoltageRegisterGroupB  = 125,
  STATUS_FUNCTION_RUN_readCellVoltageRegisterGroupC  = 126,
  STATUS_FUNCTION_RUN_readCellVoltageRegisterGroupD  = 127,
  STATUS_FUNCTION_RUN_startCellVoltageADCConversion  = 128,
  STATUS_FUNCTION_RUN_readStatusRegisterGroupA  = 129,
  STATUS_FUNCTION_RUN_readStatusRegisterGroupB  = 130,
  STATUS_FUNCTION_RUN_wakeUpSerialInterface  = 131,
  STATUS_RECEIVED_PEC_MISMATCH = 140,
  STATUS_BOARD_OUTDATED = 141
};


// Task periods and prescalars
const long    TIMER_PERIOD                      = 100000;                         // 10Hz
const long    TASK_PERIOD_CheckBattery          = 100000;     // 10Hz
const long    TASK_PERIOD_UpdateLCD             = 1000000;      // 1Hz
const long    TASK_PERIOD_BroadcastMeasurements = 1000000; // 1Hz
const long    TASK_PERIOD_BroadcastState        = 100000; // 10Hz
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
const uint8_t     START_BUTTON_PIN          = 2;  // The start button is limited to pin 2 (hardware interrupt 0)
const uint8_t     RELAY_PIN_MAIN            = 8;
const uint8_t     RELAY_PIN_CHARGE          = 4;

// SPI: LTC6804 
const uint8_t     N_DEVICES                 = 3;  // The number of 6804's that are stacked
const uint8_t     SPI_6804_CS_PIN           = 9;  // The chipSelect pin for the LTC6804 SPI connection

// SPI: CAN bus
const uint8_t     SPI_CANBUS_CS_PIN         = 10;
const uint8_t     canbusInitMaxTries        = 5;
const uint8_t canbusVoltageDataStartId  = 100;
const uint8_t canbusVoltageDebugId      = 133;
const uint8_t canbusVoltageBatteryEventId = 134;

class LTC6804Stack {
  public:
    LTC6804Stack()
    {
      resetLocalRegisters();
    }

    void resetLocalRegisters()
    {
      for (byte device = 0; device < nDevices; device++)
      {
        CFGR[device][0] = 0xD8;
        CFGR[device][1] = 0xD5;
        CFGR[device][2] = 0x16;
        CFGR[device][3] = 0xA4;
        CFGR[device][4] = 0x00;
        CFGR[device][5] = 0x00;

        for (byte i = 0; i < 6; i++)
        {
          CVAR[device][i] = 0xFF;
          CVBR[device][i] = 0xFF;
          CVCR[device][i] = 0xFF;
          CVDR[device][i] = 0xFF;
          STAR[device][i] = 0xFF;
          STBR[device][i] = 0xFF;
        }
      }
    }

    void initVoltageSensorSPI()
    {
      pinMode(chipSelectPin, OUTPUT);
      SPI.begin();

      // Need to do a couple things before communication
      // 1. Set the bit order
      // 2. Set data clock idle when high/low and set samples on rising or falling clk edges
      // 3. Speed of SPI

      SPI.setBitOrder(MSBFIRST);

      // LTC6804 Datasheet Pg. 35, "Timing": CPHA = 1 and CPOL = 1
      SPI.setDataMode(SPI_MODE3);

      // The maximum data rate for the LTC6804 using a CAT 5 twisted pair cable up to 10 meters is 1Mbps
      // (see Datasheet Pg. 35, "Timing" and pg 70, "Applications Information")
      // Arduino Nanos have a clock speed of 16 MHz. Using a clock divider of 16 will result in a SPI clock speed of 1MHz, which transfers 1Mbps
      SPI.setClockDivider(SPI_CLOCK_DIV16);
    }

    // WRCFG
    void writeConfigurationRegisterGroup()
    {
      wakeUpSerialInterface();

      canbusSend(STATUS_FUNCTION_RUN_writeConfigurationRegisterGroup);

      // Pull CSB low
      digitalWrite(chipSelectPin, LOW);

      // Send the command
      sendCommandAndPec(0x00, 0x01);

      // send the data
      for (short device = nDevices - 1; device >= 0; device--)
      {
        // Send the same configuration to all devices, starting with the highest stacked device
        sendBytesAndPEC(CFGR[device], 6);
      }

      // Pull CSB high, data is latched on rising edge
      digitalWrite(chipSelectPin, HIGH);
    }
    // RDCFG
    void readConfigurationRegisterGroup()
    {
      canbusSend(STATUS_FUNCTION_RUN_readConfigurationRegisterGroup);
      wakeUpSerialInterface();
      digitalWrite(chipSelectPin, LOW);
      sendCommandAndPec(0x00, 0x02);
      readBytesIntoRegister(CFGR);
      digitalWrite(chipSelectPin, HIGH);
    }
    void readCellVoltageRegisterGroupA()
    {
      canbusSend(STATUS_FUNCTION_RUN_readCellVoltageRegisterGroupA);
      wakeUpSerialInterface();
      digitalWrite(chipSelectPin, LOW);
      sendCommandAndPec(0x00, 0x04);
      readBytesIntoRegister(CVAR);
      digitalWrite(chipSelectPin, HIGH);
    }
    void readCellVoltageRegisterGroupB()
    {
      canbusSend(STATUS_FUNCTION_RUN_readCellVoltageRegisterGroupB);
      wakeUpSerialInterface();
      digitalWrite(chipSelectPin, LOW);
      sendCommandAndPec(0x00, 0x06);
      readBytesIntoRegister(CVBR);
      digitalWrite(chipSelectPin, HIGH);
    }
    void readCellVoltageRegisterGroupC()
    {
      canbusSend(STATUS_FUNCTION_RUN_readCellVoltageRegisterGroupC);
      wakeUpSerialInterface();
      digitalWrite(chipSelectPin, LOW);
      sendCommandAndPec(0x00, 0x08);
      readBytesIntoRegister(CVCR);
      digitalWrite(chipSelectPin, HIGH);
    }
    void readCellVoltageRegisterGroupD()
    {
      canbusSend(STATUS_FUNCTION_RUN_readCellVoltageRegisterGroupD);
      wakeUpSerialInterface();
      digitalWrite(chipSelectPin, LOW);
      sendCommandAndPec(0x00, 0x0A);
      readBytesIntoRegister(CVDR);
      digitalWrite(chipSelectPin, HIGH);
    }
    void startCellVoltageADCConversion()
    {
      canbusSend(STATUS_FUNCTION_RUN_startCellVoltageADCConversion);
      wakeUpSerialInterface();
      digitalWrite(chipSelectPin, LOW);
      sendCommandAndPec(0x03, 0x70);
      digitalWrite(chipSelectPin, HIGH);

    }
    float getCellVoltageInVolts(byte cell6804ID)
    {
      // Ensure that the cell ID provided is valid
      if (cell6804ID < 0 || cell6804ID >= nDevices * 12)
      {
        return -1;
      }
      return getCellVoltageRaw(cell6804ID) * 0.0001;
    }

    void readStatusRegisterGroupA()
    {
      canbusSend(STATUS_FUNCTION_RUN_readStatusRegisterGroupA);
      wakeUpSerialInterface();
      digitalWrite(chipSelectPin, LOW);
      sendCommandAndPec(0x00, 0x10);
      readBytesIntoRegister(STAR);
      digitalWrite(chipSelectPin, HIGH);
    }
    void readStatusRegisterGroupB()
    {
      canbusSend(STATUS_FUNCTION_RUN_readStatusRegisterGroupB);
      wakeUpSerialInterface();
      digitalWrite(chipSelectPin, LOW);
      sendCommandAndPec(0x00, 0x12);
      readBytesIntoRegister(STBR);
      digitalWrite(chipSelectPin, HIGH);

      // Check to ensure for new model board. Check bit 3 of STBR5
      for (byte boardNumber = 0; boardNumber < nDevices; boardNumber++)
      {
        if (STBR[boardNumber][5] & B00001000 == B00000000)
        {
          canbusSend(STATUS_BOARD_OUTDATED);
        }
      }

    }
    byte getRSVD(byte boardNumber)
    {
      // Returns 0 if the board is the old model, 1 if the new model.
      return STBR[boardNumber][5] && 0x08;
    }

    unsigned short getCellVoltageRaw(byte cell6804ID) // TODO: check cell6804ID for proper value first;
    {
      // Cell number goes from 0 to 12*nCells-1
      // First figure out which device the cell exists on

      char deviceNumber = cell6804ID / 12;
      char relativeCellNumber = cell6804ID % 12;

      switch (relativeCellNumber)
      {
        // Cell Voltage Register A
        case 0:
          return word(CVAR[deviceNumber][1], CVAR[deviceNumber][0]);
        case 1:
          return word(CVAR[deviceNumber][3], CVAR[deviceNumber][2]);
        case 2:
          return word(CVAR[deviceNumber][5], CVAR[deviceNumber][4]);

        // Cell Voltage Register B
        case 3:
          return word(CVBR[deviceNumber][1], CVBR[deviceNumber][0]);
        case 4:
          return word(CVBR[deviceNumber][3], CVBR[deviceNumber][2]);
        case 5:
          return word(CVBR[deviceNumber][5], CVBR[deviceNumber][4]);

        // Cell Voltage Register C
        case 6:
          return word(CVCR[deviceNumber][1], CVCR[deviceNumber][0]);
        case 7:
          return word(CVCR[deviceNumber][3], CVCR[deviceNumber][2]);
        case 8:
          return word(CVCR[deviceNumber][5], CVCR[deviceNumber][4]);

        // Cell Voltage Register D
        case 9:
          return word(CVDR[deviceNumber][1], CVDR[deviceNumber][0]);
        case 10:
          return word(CVDR[deviceNumber][3], CVDR[deviceNumber][2]);
        case 11:
          return word(CVDR[deviceNumber][5], CVDR[deviceNumber][4]);

          break;

        default:
          // Error
          return 0xFF;
      }

    }

  private:
    // Stacked devices are referenced from 0 (the bottom) to nDevices-1 (the top)
    static const byte nDevices = N_DEVICES;       // This number determines the number of 6804's that are stacked
    static const byte chipSelectPin = SPI_6804_CS_PIN;  // This number determines the chipSelect for the LTC6804 SPI connection
    byte CFGR[nDevices][6];
    byte CVAR[nDevices][6];
    byte CVBR[nDevices][6];
    byte CVCR[nDevices][6];
    byte CVDR[nDevices][6];
    byte STAR[nDevices][6];
    byte STBR[nDevices][6];

    void wakeUpSerialInterface()
    {
      canbusSend(STATUS_FUNCTION_RUN_wakeUpSerialInterface);
      delayMicroseconds(1); // Minimum length of time of SCK low is 0.2us

      // 1. Send a dummy byte. The activity on CSB and SCK will wake up the serial interface
      digitalWrite(chipSelectPin, LOW);
      sensorSend(0x00);
      digitalWrite(chipSelectPin, HIGH);

      // 2. For n devices in the stack, wait for the amount of time n * t_wake in order to power up all the devices (t_wake = 300us)
      delayMicroseconds(nDevices * 300);
    }

    // This algorithm is described on page 42 of the 6804 Datasheet
    static short calculatePEC(unsigned char* DIN, unsigned char nBytes)
    {
      // 1.Initialize the PEC to 0x0010
      short PEC = 0x0010;

      // 2. For each bit DIN coming into the PEC register group
      for (char byteNumber = 0; byteNumber < nBytes; byteNumber++)
      {
        for (char bitNumber = 7; bitNumber >= 0; bitNumber--)
        {
          // Set IN
          short IN = 0x0000;
          bitWrite(IN, 0, bitRead(DIN[byteNumber], bitNumber) ^ bitRead(PEC, 14));
          bitWrite(IN, 3, bitRead(IN, 0) ^ bitRead(PEC, 2));
          bitWrite(IN, 4, bitRead(IN, 0) ^ bitRead(PEC, 3));
          bitWrite(IN, 7, bitRead(IN, 0) ^ bitRead(PEC, 6));
          bitWrite(IN, 8, bitRead(IN, 0) ^ bitRead(PEC, 7));
          bitWrite(IN, 10, bitRead(IN, 0) ^ bitRead(PEC, 9));
          bitWrite(IN, 14, bitRead(IN, 0) ^ bitRead(PEC, 13));

          // Update 15-bit PEC
          PEC = PEC << 1;
          bitWrite(PEC, 14, bitRead(IN, 14));
          bitWrite(PEC, 10, bitRead(IN, 10));
          bitWrite(PEC, 8, bitRead(IN, 8));
          bitWrite(PEC, 7, bitRead(IN, 7));
          bitWrite(PEC, 4, bitRead(IN, 4));
          bitWrite(PEC, 3, bitRead(IN, 3));
          bitWrite(PEC, 0, bitRead(IN, 0));
        }
      }

      // 4. Append at the LSB then return
      return PEC << 1;
    }

    // Sends the data followed by the PEC on the SPI line
    void sendBytesAndPEC(byte* data, unsigned char length)
    {
      for (char i = 0; i < length; i++)
      {
        sensorSend(data[i]);
      }

      short pec = calculatePEC(data, length);
      sensorSend(highByte(pec));
      sensorSend(lowByte(pec));
    }

    // Sends a command followed by its pec
    void sendCommandAndPec(byte byte0, byte byte1)
    {
      byte bytes[2];
      bytes[0] = byte0;
      bytes[1] = byte1;
      sendBytesAndPEC(bytes, 2);
    }

    // Reads the following bytes into the registers
    void readBytesIntoRegister(byte(&registerGroup)[nDevices][6])
    {
      // Response begins with registers in lowest device in stack
      for (byte deviceNumber = 0; deviceNumber < nDevices; deviceNumber++)
      {
        // Registers arrive starting with reg0, reg1, reg2... reg5
        for (byte reg = 0; reg < 6; reg++)
        {
          byte response = sensorSend(0x00);
          registerGroup[deviceNumber][reg] = response;
        }

        // Check following PEC
        short responsePec = calculatePEC(registerGroup[deviceNumber], 6);
        byte pec0 = sensorSend(0x00);
        byte pec1 = sensorSend(0x00);
        if (highByte(responsePec) != pec0 || lowByte(responsePec) != pec1)
        {
          canbusSend(STATUS_RECEIVED_PEC_MISMATCH);
        }
      }
    }    
};

/* 
  2. MAIN PROGRAM
*/

LTC6804Stack stack;
LiquidCrystal lcd(A5, A4, A0, A1, A2, A3);
volatile CAR_STATE carState = STATE_IDLE;

volatile boolean newTimeSlice = false;
boolean task_CheckBattery_Ready = false;
boolean task_UpdateLCD_Ready = false;
boolean task_BroadcastMeasurements_Ready = false;
boolean task_BroadcastState_Ready = false;

// Status
float maxValue = 0;
float minValue = 0;
byte maxId = -1;
byte minId = -1;
uint8_t batteryStatusCode = -1;

MCP_CAN CAN(SPI_CANBUS_CS_PIN);
boolean canbusActive = false;

void setup()
{
  // Initialize relay pins
  pinMode(RELAY_PIN_MAIN, OUTPUT);
  pinMode(RELAY_PIN_CHARGE, OUTPUT);
  digitalWrite(RELAY_PIN_MAIN,LOW);
  digitalWrite(RELAY_PIN_CHARGE,LOW);

  // Start Serial Communication with Computer TODO: Replace completely with canbus
  Serial.begin(115200);

  // Initialize communication with LTC6804
  stack.initVoltageSensorSPI();
  stack.resetLocalRegisters();
  stack.writeConfigurationRegisterGroup();

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
void initCanbus()
{
  canbusActive = false;

  for (int i = 0; i < canbusInitMaxTries && canbusActive == false; i++)
  {
    if(CAN_OK == CAN.begin(CAN_1000KBPS))                   // init can bus : baudrate = 500k
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
  lcd.setCursor(0, 0);
  lcd.print("Sunstang BPS");
  lcd.setCursor(0, 1);
  lcd.print("VC R1");
}

// Communicate to LTC6804
byte sensorSend(byte b)
{
  //canbusSend("Sending: \t");
  //canbusSend(b);
  //canbusSend("\tReceiving: \t");
  switchSpiToSensor();
  byte response = SPI.transfer(b);
  //canbusSend(response);
  //canbusSend("\n");

  return response;
}

// Communicate to Canbus
void canbusSend(uint8_t statusCode)
{
  if (canbusActive)
  {
    switchSpiToCanbus();
    CAN.sendMsgBuf(canbusVoltageDebugId, 0, 1, &statusCode);  
    delay(2);
  }
}

void canbusEventSend(uint8_t cellNumber, uint8_t status)
{
  if (canbusActive)
  {
    switchSpiToCanbus();
    uint8_t message[2] = {cellNumber, status};
    CAN.sendMsgBuf(canbusVoltageBatteryEventId, 0, 2, message);
    delay(2);
  }
}

// This is specific to the final configuration
static byte get6804CellID(byte cellPosition)
{
  if (cellPosition >= 1 && cellPosition <= 11)
  {
    // Cell belongs to the bottom 6804
    return cellPosition - 1;
  }
  else if (cellPosition >= 12 && cellPosition <= 22)
  {
    // Cell belongs to the middle 6804
    return cellPosition;
  }
  else if (cellPosition >= 23 && cellPosition <= 32)
  {
    // Cell belongs to the top 6804
    return cellPosition + 1;
  }
  else
  {
    return -1;
  }
}

boolean isOvervoltage(float measuredVoltage)
{
  return measuredVoltage >= ABSOLUTE_OVER_VOLTAGE;
}

boolean isUndervoltage(float measuredVoltage)
{
  return measuredVoltage <= ABSOLUTE_UNDER_VOLTAGE;
}


boolean isFull(float measuredVoltage)
{
  return measuredVoltage >= MINIMUM_FULL_VOLTAGE;
}

void enableMainRelay()
{
  if (digitalRead(RELAY_PIN_MAIN) != HIGH) 
  {
    canbusSend(STATUS_MAIN_RELAY_ENABLED);
    digitalWrite(RELAY_PIN_MAIN, HIGH);
  }
}

void disableMainRelay()
{
  if (digitalRead(RELAY_PIN_MAIN) != LOW) 
  {
    canbusSend(STATUS_MAIN_RELAY_DISABLED);
    digitalWrite(RELAY_PIN_MAIN, LOW);
  }
}

void enableChargeRelay()
{
  if (digitalRead(RELAY_PIN_CHARGE) != HIGH) 
  {
    canbusSend(STATUS_CHARGE_RELAY_ENABLED);
    digitalWrite(RELAY_PIN_CHARGE, HIGH);
  }
}

void disableChargeRelay()
{
  if (digitalRead(RELAY_PIN_CHARGE) != LOW) 
  {
    canbusSend(STATUS_CHARGE_RELAY_DISABLED);
    digitalWrite(RELAY_PIN_CHARGE, LOW);
  }
}

void updateModuleVoltages()
{
  // Measure the cells
  stack.startCellVoltageADCConversion();
  stack.readCellVoltageRegisterGroupA();
  stack.readCellVoltageRegisterGroupB();
  stack.readCellVoltageRegisterGroupC();
  stack.readCellVoltageRegisterGroupD();
}

void startCarFunction()
{
  // Start button pressed
  // This function is the only way to enter status STATE_ACTIVE
  carState = STATE_ACTIVE;
}

void printMaxAndMin(byte maxId, float maxValue, byte minId, float minValue)
{
  lcd.clear();

  // Print static text
  lcd.setCursor(0, 0);
  lcd.print("Max:  #");
  lcd.setCursor(0, 1);
  lcd.print("Min:  #");

  // Print data
  lcd.setCursor(7, 0);
  lcd.print(maxId);
  lcd.setCursor(11, 0);
  lcd.print(maxValue);

  // Print data
  lcd.setCursor(7, 1);
  lcd.print(minId);
  lcd.setCursor(11, 1);
  lcd.print(minValue);

  // Print static text
  lcd.setCursor(15, 0);
  lcd.print("V");
  lcd.setCursor(15, 1);
  lcd.print("V");
}

// Update the minimum and maximum values.
// Used specifically for LCD printed values
static void updateMinAndMaxValues(uint8_t cellPosition, float measuredValue, byte& maxId, float& maxValue, byte& minId, float& minValue)
{
  maxValue = max(maxValue, measuredValue);
  if (measuredValue == maxValue)
  {
    maxId = cellPosition;
  }

  minValue = min(minValue, measuredValue);
  if (measuredValue == minValue)
  {
    minId = cellPosition;
  }
}

static byte checkBatteryPackStatus(byte& maxId, float& maxValue, byte& minId, float& minValue)
{
  byte batteryStatus;
  uint8_t nCellsOver = 0;
  uint8_t nCellsUnder = 0;
  uint8_t nCellsFull = 0;

  // Reset min and max values for an entire pass through all the sensors
  maxValue = ABSOLUTE_UNDER_VOLTAGE - 1;
  minValue = ABSOLUTE_OVER_VOLTAGE + 1;
  maxId = -1;
  minId = -1;

  // Scan the cells
  for (uint8_t cellPosition = 1; cellPosition <= 32; cellPosition++)
  {
    byte cellId = get6804CellID(cellPosition);
    float measuredValue = stack.getCellVoltageInVolts(cellId);

    // Update min and max values for LCD
    updateMinAndMaxValues(cellPosition, measuredValue, maxId, maxValue, minId, minValue);

    // Determine state of individual cell 
    if (isOvervoltage(measuredValue))
    {
      nCellsOver++;

      // Battery Event: Overvoltage
      canbusEventSend(cellPosition, EVENT_CODE_OVERVOLTAGE);
    }
    else if (isUndervoltage(measuredValue))
    {
      nCellsUnder++;

      // Battery Event: Undervoltage
      canbusEventSend(cellPosition, EVENT_CODE_UNDERVOLTAGE);
    }
    else if (isFull(measuredValue))
    {
      nCellsFull++;
    }
  }

  // Determine overall battery state using the total number
  // of overvoltage cells, undervoltage cells, and full cells.
  if (nCellsUnder > 0 && nCellsOver > 0)
  {
    // Battery event: There are both undervoltage and overvoltage cells
    batteryStatus = STATUS_CODE_UNDEROVERVOLTAGE;
  }
  else if (nCellsOver > 0)
  {
    // Battery event: There are overvoltage cells
    batteryStatus = STATUS_CODE_OVERVOLTAGE;
  }
  else if (nCellsUnder > 0)
  {
    // There are undervoltage cells
    batteryStatus = STATUS_CODE_UNDERVOLTAGE;
  }
  else if (nCellsFull > 0)
  {
    // Stop charging cells
    batteryStatus = STATUS_CODE_FULL;
  }
  else
  {
    // Everything OK
    batteryStatus = STATUS_CODE_NEEDS_CHARGING;
  }

  return batteryStatus;
}

void switchSpiToCanbus()
{
  // Need to do a couple things before communication
  // 1. Set the bit order
  // 2. Set data clock idle when high/low and set samples on rising or falling clk edges
  // 3. Speed of SPI

  // CAN uses the defualt SPI.begin()

  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
}

void switchSpiToSensor()
{
  // Need to do a couple things before communication
  // 1. Set the bit order
  // 2. Set data clock idle when high/low and set samples on rising or falling clk edges
  // 3. Speed of SPI

  SPI.setBitOrder(MSBFIRST);

  // LTC6804 Datasheet Pg. 35, "Timing": CPHA = 1 and CPOL = 1
  SPI.setDataMode(SPI_MODE3);

  // The maximum data rate for the LTC6804 using a CAT 5 twisted pair cable up to 10 meters is 1Mbps
  // (see Datasheet Pg. 35, "Timing" and pg 70, "Applications Information")
  // Arduino Nanos have a clock speed of 16 MHz. Using a clock divider of 16 will result in a SPI clock speed of 1MHz, which transfers 1Mbps
  SPI.setClockDivider(SPI_CLOCK_DIV16);
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
    canbusSend(STATUS_TASK_CHECKBATTERY_RUN_ACTIVE);

    updateModuleVoltages();

    batteryStatusCode = checkBatteryPackStatus(maxId, maxValue, minId, minValue);

    switch (batteryStatusCode)
    {
      case STATUS_CODE_NEEDS_CHARGING:
        enableMainRelay();
        enableChargeRelay();
        break;
      case STATUS_CODE_FULL:
        enableMainRelay();
        disableChargeRelay();
        break;
      case STATUS_CODE_UNDERVOLTAGE:
        carState = STATE_IDLE;
        break;
      case STATUS_CODE_OVERVOLTAGE:
        carState = STATE_IDLE;
        break;
      case STATUS_CODE_UNDEROVERVOLTAGE:
        carState = STATE_IDLE;
      default:
        carState = STATE_IDLE;
        break;
    }
  }
  else
  {
    // IDLE state: Deactivate all functions and do nothing
    canbusSend(STATUS_TASK_CHECKBATTERY_RUN_IDLE);
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

  if (currentFrame < firstFrameLength)
  {
    // Display the first screen
    printMaxAndMin(maxId, maxValue, minId, minValue);
  }
  else
  {
    // Display the second screen
    switch (batteryStatusCode)
    {
      case STATUS_CODE_NEEDS_CHARGING:
        printMaxAndMin(maxId, maxValue, minId, minValue);
        break;
      case STATUS_CODE_FULL:
        lcd.setCursor(0, 0);
        lcd.print("Battery Full    ");
        break;
      case STATUS_CODE_OVERVOLTAGE:
        lcd.setCursor(0, 0);
        lcd.print("Overvoltage     ");
        break;
      case STATUS_CODE_UNDERVOLTAGE:
        lcd.setCursor(0, 0);
        lcd.print("Undervoltage    ");
        break;
      case STATUS_CODE_UNDEROVERVOLTAGE:
        lcd.setCursor(0, 0);
        lcd.print("UnderOverVoltage");
        break;
      default:
        break;
    }
  }

  if (currentFrame >= firstFrameLength + secondFrameLength - 1)
  {
    currentFrame = 1;
  }

  currentFrame++;
}

void task_BroadcastMeasurements()
{
  if (canbusActive)
  {
    // Switch SPI Settings
    switchSpiToCanbus();
    
    for (int cell = 1; cell <= 32; cell+=4) // TODO: Make the loop work with any value of nSensors
    {
      uint16_t v1 = stack.getCellVoltageRaw(get6804CellID(cell));
      uint16_t v2 = stack.getCellVoltageRaw(get6804CellID(cell+1));
      uint16_t v3 = stack.getCellVoltageRaw(get6804CellID(cell+2));
      uint16_t v4 = stack.getCellVoltageRaw(get6804CellID(cell+3));
      
      unsigned char array[8] = {
        highByte(v1), 
        lowByte(v1),
        highByte(v2), 
        lowByte(v2),
        highByte(v3), 
        lowByte(v3),
        highByte(v4), 
        lowByte(v4)
      };
      uint8_t canbusDataID = canbusVoltageDataStartId + (cell-1)/4;
      CAN.sendMsgBuf(canbusDataID, 0, 8, array);  
      delay(2);
    }
  }
}

void task_BroadcastState()
{
  if (carState == STATE_ACTIVE)
  {
    canbusSend(STATUS_TASK_BROADCASTSTATE_ACTIVE);
  }
  else
  {
    canbusSend(STATUS_TASK_BROADCASTSTATE_IDLE);
  }
}

