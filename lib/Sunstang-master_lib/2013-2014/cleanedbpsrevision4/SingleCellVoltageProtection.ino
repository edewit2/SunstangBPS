// Sunstang 2015: Single-cell voltage protection for PL-9759156-5C
// Sept 17 2014
// John Abanador

#include <LiquidCrystal.h>
#include <OneWire.h>

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
int PIN_RELAY = 13;
const byte    PIN_SENSOR = 6;
const float    VOLTAGE_PER_INT16     = 4.883 / 1000; // V/int
const float    ABSOLUTE_OVER_VOLTAGE                 = 4.15; // Threshold: 4.15 V (Units: 4.883 mV/integer)
const float    ABSOLUTE_UNDER_VOLTAGE                = 2.80; // Threshold: 2.80 V (Units: 4.883 mV/integer)
const float    WARN_MINIMUM_ARDUINO_VIN = 7.00;
const float    UPPER_SAFETY_PERCENTAGE     = 0.94;
const float    LOWER_SAFETY_PERCENTAGE     = 2 - UPPER_SAFETY_PERCENTAGE;
OneWire  ds(PIN_SENSOR);

float cellLevel;
float meterBatteryLevel;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.clear();
  pinMode(PIN_RELAY, OUTPUT); 
  
  // Check battery before powering on
  checkBattery();
  
  setRelay(HIGH);
}

void loop() {
  delay(500);
  checkBattery();
}

void checkBattery(){

  // Read sensor
  requestFromSensorAndReadData();
  
  // Output information
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("V =            V");
  lcd.setCursor(4, 0);
  lcd.print(cellLevel);
  
  if (isMeterLowBattery()){
    isolateBattery("Meter batt low");
  }
  
  if (isCellUndervoltage()){
    isolateBattery("Cell undervoltage");
  } else if (isCellOvervoltage()){
    isolateBattery("Cell overvoltage");
  }
  
}

void requestFromSensorAndReadData(){
  
    // Step 1
    ds.reset();
    
    // Step 2
    ds.skip();
    
    // Step 3
    ds.write(0x69);
    ds.write(0x0C);

    byte msb = ds.read();
    byte lsb = ds.read();
    Serial.println(msb, HEX);
    Serial.println(lsb, HEX);
    cellLevel = convertToLogicalLevel(word(msb, lsb));
    
    for (int i = 0; i < 14; i++) {
        ds.read(); // throw away data
    }

    msb = ds.read();
    lsb = ds.read();
    Serial.println(msb, HEX);
    Serial.println(lsb, HEX);
    float voltageDifference = convertToLogicalLevel(word(msb, lsb));
    meterBatteryLevel = voltageDifference + cellLevel;
    
    // End
    ds.reset();
}


void isolateBattery(String message) {
    setRelay(LOW);
    
    lcd.setCursor(0,1);
    lcd.print(message);
    while(true);
}
void setRelay(bool state){
  digitalWrite(PIN_RELAY, state);
}
bool isCellUndervoltage() {
    return (cellLevel < ABSOLUTE_UNDER_VOLTAGE);
}

bool isCellOvervoltage() {
    return (cellLevel > ABSOLUTE_OVER_VOLTAGE);
}

bool isMeterLowBattery(){
    return (meterBatteryLevel < WARN_MINIMUM_ARDUINO_VIN);
}

float convertToLogicalLevel(int16_t sensorValue){
   return (sensorValue / 32) * VOLTAGE_PER_INT16;
}
