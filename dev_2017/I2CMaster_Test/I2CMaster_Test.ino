#include <Wire.h>
#include <TimerOne.h>

// Digital Pinout
// Analog Pinout
  
// Global variables
const uint8_t NTemperatureBoards = 8;   // number of sensor boards (one per rack)
const uint8_t NTemperatureSensors = 8;  // number of thermistors per sensor board
const uint8_t NCurrentBoards = 1;  // number of current sensors
float temperature[NTemperatureBoards][NTemperatureSensors] = {0};
float current[NCurrentBoards] = {0};
volatile boolean tick = false;
uint8_t getCurrentCount = 0;
uint8_t getTemperatureCount = 0;

// I2C sensor IDs
const uint8_t temperatureBoardID[NTemperatureBoards] = {8,9,10,11,12,13,14,15}; // I2C addresses of temperature boards
const uint8_t currentBoardID[NCurrentBoards] = {16};  // I2C address of current sensor board

void setup() {   
  Serial.begin(9600);  
  Wire.begin(); // join i2c bus (address optional for master)

  Timer1.initialize(50000); // 50ms period
  Timer1.attachInterrupt(timerEvent); // timer interrupt callback function
}

void loop() {
  waitForTimerEvent();
}

// Timer1 interrupt service routine
void timerEvent(void) {
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
void getTemperature(uint8_t ID, uint8_t n_bytes, uint8_t boardNumber) {
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
void getCurrent(uint8_t ID, uint8_t n_bytes, uint8_t boardNumber) {
  Serial.println("***** CURRENT MEASUREMENT *****");
  Wire.requestFrom(ID,n_bytes);
  while (Wire.available()) {
    current[boardNumber] = Wire.read()/2.8;
    Serial.print("Reading current device: ");Serial.println(boardNumber);
    Serial.print("Current: ");Serial.println(current[boardNumber]);
  }
  Serial.println();
}


