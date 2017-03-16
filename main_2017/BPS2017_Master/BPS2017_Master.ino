#include <Wire.h>
#include <TimerOne.h>

// Digital Pinout
int startButton_pin = 2;       // Start button connected to digital pin 2 (hardware interrupt 0)
int fanTach_pin = 3;     // Fan tach feedback connected to digital pin 3 (hardware interrupt 1)
int fanPwm_pin = 5;      // Mosfet connected to digital pin 5 (pwm)
int relayBattery_pin = 6;      // Relay connected to digital pin 6
int relayMotor_pin = 7;        // Relay connected to digital pin 7
int relayArray_pin = 8;        // Relay connected to digital pin 8

// Analog Pinout
int analogA0_pin = 0;

// Definitions
const float OVERTEMPERATURE_CHARGE = 45;  // 45 degrees celcius
const float OVERTEMPERATURE_DISCHARGE = 60; // 60 degrees celcius
const float OVERCURRENT_CHARGE = 130; // 130 amperes
const float OVERCURRENT_DISCHARGE = -15; // 15 amperes
const float OVER_VOLTAGE = 4.15; // 4.15 volts
const float CELL_VOLTAGE = 4.00; // 4.00 volts
const float UNDER_VOLTAGE = 2.85; // 2.85 volts

// Global variables
const uint8_t NTemperatureBoards = 8;   // number of sensor boards (one per rack)
const uint8_t NTemperatureSensors = 8;  // number of thermistors per sensor board
const uint8_t NCurrentBoards = 1;  // number of current sensors
float temperature[NTemperatureBoards][NTemperatureSensors] = {0};
float current[NCurrentBoards] = {0};
volatile boolean tick = false;
uint8_t getCurrentCount = 0;
uint8_t getTemperatureCount = 0;
int pwmVal = 0;

// I2C sensor IDs
const uint8_t temperatureBoardID[NTemperatureBoards] = {8,9,10,11,12,13,14,15}; // I2C addresses of temperature boards
const uint8_t currentBoardID[NCurrentBoards] = {16};  // I2C address of current sensor board
const uint8_t breakoutBoardID = 17;

enum CAR_STATE {
  STATE_IDLE,
  STATE_ACTIVE
};

volatile CAR_STATE carState = STATE_IDLE;
volatile byte half_revolutions = 0;

void setup() 
{     
  initSystem();
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)

  Timer1.initialize(50000);  // 50ms period
  Timer1.attachInterrupt(timerEvent); // timer interrupt callback function
  
  attachInterrupt(digitalPinToInterrupt(startButton_pin),startCarFunction, FALLING);
}

void loop() 
{
  if (carState == STATE_ACTIVE) {
    waitForTimerEvent();
  }
}

void initSystem(void) 
{
  pinMode(relayBattery_pin, OUTPUT);
  analogWrite(fanPwm_pin,123);  // set fan speed to 50% duty cycle (0-255)
}

// Timer1 interrupt service routine
void timerEvent(void) 
{
  tick = true;  // timer overflow indicator
}

void waitForTimerEvent(void)
{
  while(tick == false); tick = false;
  getTemperatureCount++;
  getCurrentCount++;
  Serial.println(getTemperatureCount);
  Serial.println(getCurrentCount);
  
  if (getTemperatureCount >= 10) {
    getTemperatureCount = 0;
    getTemperature(temperatureBoardID[0],NTemperatureSensors,0);
  }
  if (getCurrentCount >= 20) {
    getCurrentCount = 0;
    getCurrent(currentBoardID[0],NCurrentBoards,0);
  }
}

// Read temperature sensor device
void getTemperature(uint8_t ID, uint8_t n_bytes, uint8_t boardNumber) 
{
  Serial.println("***** TEMPERATURE MEASUREMENT *****");
  uint8_t i = 0;
  Wire.requestFrom(ID,n_bytes); // this takes t = 8*(1 + n_bytes)*NTemperatureSensors seconds
  while (Wire.available()) {
    temperature[boardNumber][i] = Wire.read()/4.0;
    Serial.print("Reading device ");Serial.print(boardNumber);
    Serial.print(", Sensor number ");Serial.println(i);
    Serial.print("Temperature: ");Serial.println(temperature[boardNumber][i]);
    i++;
  }
  Serial.println();
}

// Read current sensor device
void getCurrent(uint8_t ID, uint8_t n_bytes, uint8_t boardNumber) 
{
  Serial.println("***** CURRENT MEASUREMENT *****");
  Wire.requestFrom(ID,n_bytes);
  while (Wire.available()) {
    current[boardNumber] = Wire.read()/2.8;
    Serial.print("Reading current device: ");Serial.println(boardNumber);
    Serial.print("Current: ");Serial.println(current[boardNumber]);
  }
  Serial.println();
}

// Start button hardware interrupt
void startCarFunction(void) 
{
  carState = STATE_ACTIVE;  // only way to set carState active
  Serial.println("active!");
}

unsigned int measureRpm(void)
{
  unsigned int result = 0;
  unsigned long timeold = 0;
  half_revolutions = 0;
  
  timeold = millis();  
  attachInterrupt(digitalPinToInterrupt(fanTach_pin),rpm_fun,FALLING);
  while (half_revolutions < 20); // wait for 20 revolutions  
  result = 30*1000/(millis() - timeold)*half_revolutions;
  detachInterrupt(digitalPinToInterrupt(fanTach_pin));
  return result;
}

void rpm_fun(void)
{
  half_revolutions++;
}



