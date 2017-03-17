#include <OneWire.h>
#pragma region vars

OneWire  ds(10);

const int      TIMEOUT_BATTERY_SENSOR_DS2776 = 100; // Milliseconds

const float    NOMINAL_VOLTAGE       = 3.7;        // V -- raw value 
const float    NOMINAL_TEMPERATURE   = 20;         // Degrees Celcius
const float    VOLTAGE_PER_INT16     = 4.883 / 1000; // V/int
const float    TEMPERATURE_PER_INT16 = 0.125;      // Degrees Celcius/int
const float    R_SNS                 = 1.5 / 1000; // Sense Resistance (ohms)
const float    CURRENT_PER_INT16     = 1.5625 / (R_SNS * 1000000);  

// Voltage and temperature values need to be multiplied by 32 because the raw data's bits are left-shifted 5 bits over (2^5)
const int16_t  ABSOLUTE_OVER_VOLTAGE                 = 4.15 * 32 / VOLTAGE_PER_INT16; // Threshold: 4.15 V (Units: 4.883 mV/integer)
const int16_t  ABSOLUTE_UNDER_VOLTAGE                = 2.80 * 32 / VOLTAGE_PER_INT16; // Threshold: 2.80 V (Units: 4.883 mV/integer)
const int16_t  ABSOLUTE_OVER_TEMPERATURE_CHARGING    = 45   * 32 / TEMPERATURE_PER_INT16; // Threshold: 45 Celcius (Units: 0.125 Celcius/integer)
const int16_t  ABSOLUTE_OVER_TEMPERATURE_DISCHARGING = 60   * 32 / TEMPERATURE_PER_INT16; // Threshold: 60 Celcius (Units: 0.125 Celcius/integer)
const int16_t  ABSOLUTE_OVER_CURRENT                 = 27 / CURRENT_PER_INT16; // Threshold: 27 A (Units: (1.5625uV/Rsns)/integer, Rsns = 1.5mOhms)

const float    UPPER_SAFETY_PERCENTAGE     = 0.9;
const float    LOWER_SAFETY_PERCENTAGE     = 2 - UPPER_SAFETY_PERCENTAGE;


// These are the pins at which the arduino outputs control signals to the relays
const byte     PIN_RELAY_CHARGING    = 7; // TODO:  Set pins --> ask Dan/Gordon
const byte     PIN_RELAY_DISCHARGING = 5;
const byte     PIN_RELAY_MAIN        = 3;

// One wire
const byte    PIN_SENSOR = 12;

//These are the output pins for the led display
const byte    PIN_DISP_CLK          = 8;
const byte     PIN_DISP_LATCH       = 9;
const byte     PIN_DISP_SERIAL      = 10;

const uint16_t OV_CRITICAL  = 0x0040;
const uint16_t OV_WARN      = 0x0080;
const uint16_t UV_CRITICAL  = 0x0100;
const uint16_t UV_WARN      = 0x0200;
const uint16_t OC_CRITICAL  = 0x0400;
const uint16_t OC_WARN      = 0x0800;
const uint16_t OT_CRITICAL  = 0x1000;
const uint16_t OT_WARN      = 0x2000;
const uint16_t CHARGING     = 0x4000;
const uint16_t DISCHARGING  = 0x8000;

const bool RELAY_ON         = HIGH;
const bool RELAY_OFF        = LOW;

const bool READ_SUCCESS     = true;
const bool READ_FAILURE     = false;

boolean overCurrentOccured  = false;
boolean overCurrentWarning  = false;

boolean underVoltageOccured = false;
boolean underVoltageWarning = false;

boolean overVoltageOccured  = false;
boolean overVoltageWarning  = false;

boolean overTempOccured     = false;
boolean overTempWarning     = false;

// The local copies of the sensor measurements. These variables form the model of the battery
// IMPORTANT NOTE: these are raw data values
int16_t voltage_cell_0;
int16_t voltage_cell_1;
int16_t temperature;
int16_t current_instantaneous;
int16_t current_average;

#pragma endregion

void setup(void) {
  Serial.begin(9600);
  
  // initialize killCar indicator
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);

  // initialize relay outputs
  pinMode(PIN_RELAY_MAIN, OUTPUT);
  pinMode(PIN_RELAY_DISCHARGING, OUTPUT);
  pinMode(PIN_RELAY_CHARGING, OUTPUT);
  digitalWrite(PIN_RELAY_CHARGING, RELAY_ON);
  digitalWrite(PIN_RELAY_DISCHARGING, RELAY_ON);
  digitalWrite(PIN_RELAY_MAIN, RELAY_ON);
}

// Peter's function
void batteryCheck(void) {

    boolean charge = false;
    boolean discharge = false;

    //perform BPS functions
    overCurrentOccured |= isOvercurrent(); //Set the kill flag if we are over current
    if (overCurrentOccured) {
        killCar();
    }
    //Work out of any of the batterys meet our error conditions and set the flag if they do
    underVoltageOccured |= isUndervoltage();
    overVoltageOccured |= isOvervoltage();
    overTempOccured |= isOvertemperature();

    if (underVoltageOccured || overVoltageOccured || overTempOccured) {
        isolateBattery();
    }
    
    //Perform BMS functionality
    overCurrentWarning = isWarnOvercurrent(); //Set the kill flag if we are over current
    underVoltageWarning = isWarnUndervoltage();
    overVoltageWarning = isWarnOvervoltage();
    overTempWarning = isWarnOvertemperature();

    //Calculate if we should be charging or not
    charge = !overVoltageWarning && !overCurrentWarning && !overTempWarning && !overCurrentOccured && !overVoltageOccured && !underVoltageOccured && !overTempOccured;
    discharge = !underVoltageWarning  && !overCurrentWarning && !overTempWarning && !overCurrentOccured && !overVoltageOccured && !underVoltageOccured && !overTempOccured;


    if (charge) {
        setChargingRelay(RELAY_ON);
    } else {
        setChargingRelay(RELAY_OFF);
    }

    if (discharge) {
        setDischargingRelay(RELAY_ON);
    } else { 
        setDischargingRelay(RELAY_OFF);
    }

    //Update the Driver Display
    uint16_t leds = 0;
    
    if(overCurrentOccured)
      leds |= OC_CRITICAL;
    if(underVoltageOccured)
      leds |= UV_CRITICAL;
    if(overVoltageOccured)
      leds |= OV_CRITICAL;
    if(overTempOccured)
      leds |= OT_CRITICAL;
      
    if(overCurrentWarning)
      leds |= OC_WARN;
    if(underVoltageWarning)
      leds |= UV_WARN;
    if(overVoltageWarning)
      leds |= OV_WARN;
    if(overTempOccured)
      leds |= OT_CRITICAL;
      
    if(charge)
      leds |=CHARGING;
    if(discharge)
      leds |= DISCHARGING;
      //TODO Voltmeter display.
      
      digitalWrite(PIN_DISP_LATCH, LOW);
      shiftOut(PIN_DISP_SERIAL, PIN_DISP_CLK, MSBFIRST, highByte(leds));
      shiftOut(PIN_DISP_SERIAL, PIN_DISP_CLK, MSBFIRST, lowByte(leds));      
      digitalWrite(PIN_DISP_LATCH, HIGH);
}

// End Peter's function
void loop(void) {
      
      requestFromSensorAndReadData();
      batteryCheck();
}


  
bool requestFromSensorAndReadData(){

    
    // Step 1
    ds.reset();
    
    // Step 2
    ds.skip();
    
    // Step 3
    ds.write(0x69);
    ds.write(0x08);
    
    // Step 4
    byte msb = ds.read();
    byte lsb = ds.read();
    Serial.println(msb, HEX); //QUESTION: Are the serial lines tied to the onewire device??? Or is onewire on pin 10?
    Serial.println(lsb, HEX);
    current_average = word(msb, lsb);

    msb = ds.read();
    lsb = ds.read();
    Serial.println(msb, HEX);
    Serial.println(lsb, HEX);
    temperature = word(msb, lsb);

    msb = ds.read();
    lsb = ds.read();
    Serial.println(msb, HEX);
    Serial.println(lsb, HEX);
    voltage_cell_0 = word(msb, lsb);

    msb = ds.read();
    lsb = ds.read();
    Serial.println(msb, HEX);
    Serial.println(lsb, HEX);
    current_instantaneous = word(msb, lsb);

    for (int i = 0; i < 12; i++) {
        ds.read(); // throw away data
    }

    msb = ds.read();
    lsb = ds.read();
    Serial.println(msb, HEX);
    Serial.println(lsb, HEX);
    voltage_cell_1 = word(msb, lsb);
    
    // End
    ds.reset();
  
    
}

// End Dustin's function



// John + Peter's Functions
bool isCharging() {
    return current_instantaneous < 0;
}

// Returns true if sensor data of vCell1 or vCell2 is above our allowable limit
bool isOvervoltage() {

    //digitalWrite(LED_BUILTIN, overTempOccured); // off
    return (voltage_cell_0 > ABSOLUTE_OVER_VOLTAGE || voltage_cell_1 > ABSOLUTE_OVER_VOLTAGE);
}


// Returns true if sensor data of vCell1 or vCell2 is in the danger zone
bool isWarnOvervoltage() {
    return (voltage_cell_0 > ABSOLUTE_OVER_VOLTAGE*UPPER_SAFETY_PERCENTAGE || voltage_cell_1 > ABSOLUTE_OVER_VOLTAGE*UPPER_SAFETY_PERCENTAGE);
}


// Returns true if sensor data of vCell1 or vCell2 is below our allowable limit
bool isUndervoltage() {
    return (voltage_cell_0 < ABSOLUTE_UNDER_VOLTAGE || voltage_cell_1 < ABSOLUTE_UNDER_VOLTAGE);
}


// Returns true if sensor data of vCell1 or vCell2 is in our low end danger zone
bool isWarnUndervoltage() {
    return (voltage_cell_0 < ABSOLUTE_UNDER_VOLTAGE*LOWER_SAFETY_PERCENTAGE || voltage_cell_1 < ABSOLUTE_UNDER_VOLTAGE*LOWER_SAFETY_PERCENTAGE);
}


// Returns true if sensor data is above our allowable temperature
bool isOvertemperature() {
    return (isCharging() && temperature > ABSOLUTE_OVER_TEMPERATURE_CHARGING)
        || (!isCharging() && temperature > ABSOLUTE_OVER_TEMPERATURE_DISCHARGING);
}


// Returns true if sensor data is in our temperature danger zone
bool isWarnOvertemperature() {
    return (isCharging() && temperature > ABSOLUTE_OVER_TEMPERATURE_CHARGING*UPPER_SAFETY_PERCENTAGE)
        || (!isCharging() && temperature > ABSOLUTE_OVER_TEMPERATURE_DISCHARGING*UPPER_SAFETY_PERCENTAGE);
}


// Returns true if sensor data shows current above the absolute value
bool isOvercurrent() {
    return (abs(current_instantaneous) > ABSOLUTE_OVER_CURRENT);
}


//returns if we're in the danger zone for current
bool isWarnOvercurrent() {
    return (abs(current_instantaneous) > ABSOLUTE_OVER_CURRENT*UPPER_SAFETY_PERCENTAGE);
}


// alternate suggested implementation? --> allows explicit state,
// without necessity to define a new function for each state
void setChargingRelay(bool relayState) {
    digitalWrite(PIN_RELAY_CHARGING, relayState);
}


void setDischargingRelay(bool relayState) {
    digitalWrite(PIN_RELAY_DISCHARGING, relayState);
}


void setMainRelay(bool relayState) {
    digitalWrite(PIN_RELAY_MAIN, relayState);
}


//This will take the battery out of the main circuit
void isolateBattery() {
    setChargingRelay(RELAY_OFF);
    setDischargingRelay(RELAY_OFF);
}


//This disconnects all power sources from the power bus
void killCar() {
    setMainRelay(RELAY_OFF);
    setChargingRelay(RELAY_OFF);
    setDischargingRelay(RELAY_OFF);
}

// End John + Peter's functions
