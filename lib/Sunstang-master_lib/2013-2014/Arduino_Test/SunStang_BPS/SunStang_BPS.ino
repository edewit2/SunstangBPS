#include <DS2776_Enums.h>
#include <Packets_BPS.h>
#include <DS2776.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <SlaveNode.h>
#include <Packets_Test.h>
#include <Packet.h>
#include <I2cNetworkSlave.h>
#include <I2cNetworkMaster.h>
#include "TimerOne.h"
#include "Arduino.h"

#pragma region constants variables and basic functions

using namespace Packet_BPS;

/*
NOTE: added static const void func pointer in TimedFunction class --> remove if causes problems.
*/

// Voltage and temperature values need to be multiplied by 32 because the raw data's bits are left-shifted 5 bits over (2^5)
const int16_t ABSOLUTE_OVER_VOLTAGE                 = 4.15 * 32 / VOLTAGE_PER_INT16; // Threshold: 4.15 V (Units: 4.883 mV/integer)
const int16_t ABSOLUTE_UNDER_VOLTAGE                = 2.80 * 32 / VOLTAGE_PER_INT16; // Threshold: 2.80 V (Units: 4.883 mV/integer)
const int16_t ABSOLUTE_OVER_TEMPERATURE_CHARGING    = 45  * 32 / TEMPERATURE_PER_INT16; // Threshold: 45 Celcius (Units: 0.125 Celcius/integer)
const int16_t ABSOLUTE_OVER_TEMPERATURE_DISCHARGING = 60  * 32 / TEMPERATURE_PER_INT16; // Threshold: 60 Celcius (Units: 0.125 Celcius/integer)
const int16_t ABSOLUTE_OVER_CURRENT                 = 27 / CURRENT_PER_INT16; // Threshold: 27 A (Units: (1.5625uV/Rsns)/integer, Rsns = 1.5mOhms)

const float   UPPER_SAFETY_PERCENTAGE     = 0.9;
const float   LOWER_SAFETY_PERCENTAGE     = 2 - UPPER_SAFETY_PERCENTAGE;

//This is how often to poll the sensors in us
const long    POLL_PERIOD           = 440000;

// These are the pins at which the arduino outputs control signals to the relays
const byte    PIN_RELAY_CHARGING    = 6; // TODO:  Set pins --> ask Dan/Gordon
const byte    PIN_RELAY_DISCHARGING = 5;
const byte    PIN_RELAY_MAIN        = 4;

//These are the output pins for the led display
const byte    PIN_DISP_CLK          = 7;
const byte    PIN_DISP_LATCH        = 8;
const byte    PIN_DISP_SERIAL       = 9;

const uint8_t MAIN_SENSOR = 0;

const uint16_t OV_CRITICAL  =0x0040;
const uint16_t OV_WARN      =0x0080;
const uint16_t UV_CRITICAL  =0x0100;
const uint16_t UV_WARN      =0x0200;
const uint16_t OC_CRITICAL  =0x0400;
const uint16_t OC_WARN      =0x0800;
const uint16_t OT_CRITICAL  =0x1000;
const uint16_t OT_WARN      =0x2000;
const uint16_t CHARGING     =0x4000;
const uint16_t DISCHARGING  =0x8000;


#pragma endregion

DS2776 sensor[] = { // 16
    DS2776(0x3D5BF300600700D9ULL) // main sensor       --> {Voltage, Current, Temperature}
    //DS2776(0x3D0BF000600700B9ULL), // auxiliary sensors --> {Voltage, Temperature}
    //DS2776(0x3D07F000600700C4ULL),
    //DS2776(0x3DE2EF0060070015ULL),
    //DS2776(0x3D27EF0060070061ULL),
    //DS2776(0x3DD9EE00600700CDULL),
    //DS2776(0x3D50EF006007007CULL),
    //DS2776(0x3DC3EF0060070094ULL),
    //DS2776(0x3DA7F300600700D6ULL),
    //DS2776(0x3DDCEE0060070026ULL),
    //DS2776(0x3D9DF20060070039ULL),
    //DS2776(0x3DE1EF006007004CULL),
    //DS2776(0x3D98EF0060070042ULL),
    //DS2776(0x3Db9F1006007001DULL),
    //DS2776(0x3D53EF0060070025ULL),
    //DS2776(0x3D97EF0060070066ULL)

    /*
    DS2776(0x3DC0EF00600700CD),
    DS2776(0x3DE0EF006007007B),
    DS2776(0x3D2CEF0060070099),
    DS2776(0x3DC8F200600700FC),
    DS2776(0x3DDEEF0060070085),
    */
};

#pragma region variables and basic functions

const int NUM_SENSORS = sizeof(sensor) / sizeof(sensor[0]);

boolean overCurrentOccured  = false;
boolean overCurrentWarning  = false;

boolean underVoltageOccured = false;
boolean underVoltageWarning = false;

boolean overVoltageOccured  = false;
boolean overVoltageWarning  = false;

boolean overTempOccured     = false;
boolean overTempWarning     = false;

// Returns true if sensor data of vCell1 or vCell2 is above our allowable limit
bool isOvervoltage(DS2776 &sensor) {
    return (sensor.voltage_cell_0 > ABSOLUTE_OVER_VOLTAGE || sensor.voltage_cell_1 > ABSOLUTE_OVER_VOLTAGE);
}

// Returns true if sensor data of vCell1 or vCell2 is in the danger zone
bool isWarnOvervoltage(DS2776 &sensor) {
    return (sensor.voltage_cell_0 > ABSOLUTE_OVER_VOLTAGE*UPPER_SAFETY_PERCENTAGE || sensor.voltage_cell_1 > ABSOLUTE_OVER_VOLTAGE*UPPER_SAFETY_PERCENTAGE);
}

// Returns true if sensor data of vCell1 or vCell2 is below our allowable limit
bool isUndervoltage(DS2776 &sensor) {
    return (sensor.voltage_cell_0 < ABSOLUTE_OVER_VOLTAGE || sensor.voltage_cell_1 < ABSOLUTE_UNDER_VOLTAGE);
}

// Returns true if sensor data of vCell1 or vCell2 is in our low end danger zone
bool isWarnUndervoltage(DS2776 &sensor) {
    return (sensor.voltage_cell_0 < ABSOLUTE_OVER_VOLTAGE*LOWER_SAFETY_PERCENTAGE || sensor.voltage_cell_1 < ABSOLUTE_UNDER_VOLTAGE*LOWER_SAFETY_PERCENTAGE);
}

// Returns true if sensor data is above our allowable temperature
bool isOvertemperature(DS2776 &sensor) {
    return (sensor.isCharging() && sensor.temperature > ABSOLUTE_OVER_TEMPERATURE_CHARGING)
        || (!sensor.isCharging() && sensor.temperature > ABSOLUTE_OVER_TEMPERATURE_DISCHARGING);
}

// Returns true if sensor data is in our temperature danger zone
bool isWarnOvertemperature(DS2776 &sensor) {
    return (sensor.isCharging() && sensor.temperature > ABSOLUTE_OVER_TEMPERATURE_CHARGING*UPPER_SAFETY_PERCENTAGE)
        || (!sensor.isCharging() && sensor.temperature > ABSOLUTE_OVER_TEMPERATURE_DISCHARGING*UPPER_SAFETY_PERCENTAGE);
}

// Returns true if sensor data shows current above the absolute value
bool isOvercurrent(DS2776 &sensor) {
    return (sensor.current_instantaneous > ABSOLUTE_OVER_CURRENT) || (-sensor.current_instantaneous > ABSOLUTE_OVER_CURRENT);
}

//returns if we're in the danger zone for current
bool isWarnOvercurrent(DS2776 &sensor) {
    return (abs(sensor.current_instantaneous) > ABSOLUTE_OVER_CURRENT*UPPER_SAFETY_PERCENTAGE);
}

// alternate suggested implementation? --> allows explicit state,
// without necessity to define a new function for each state
void setChargingRelay(enum RelayState relayState) {
    digitalWrite(PIN_RELAY_CHARGING, relayState);
}

void setDischargingRelay(enum RelayState relayState) {
    digitalWrite(PIN_RELAY_DISCHARGING, relayState);
}

void setMainRelay(enum RelayState relayState) {
    digitalWrite(PIN_RELAY_MAIN, relayState);
}

//This will take the battery out of the main circuit
void isolateBattery() {
    setChargingRelay(OPEN);
    setDischargingRelay(OPEN);
}

//This disconnects all power sources from the power bus
void killCar() {
    setMainRelay(OPEN);
    setChargingRelay(OPEN);
    setDischargingRelay(OPEN);
}

#pragma endregion

void batteryCheck(void) {

    digitalWrite(13, !digitalRead(13));

    boolean charge = false;
    boolean discharge = false;

    //perform BPS functions
    for (int i = 0; i < NUM_SENSORS; i++) {

        //We only have curented sensed on the first sensor
        if (i == 0) {
            overCurrentOccured |= isOvercurrent(sensor[i]); //Set the kill flag if we are over current
            if (overCurrentOccured) {
                killCar();
            }
        }
        //Work out of any of the batterys meet our error conditions and set the flag if they do
        underVoltageOccured |= isUndervoltage(sensor[i]);
        overVoltageOccured |= isOvervoltage(sensor[i]);
        overTempOccured |= isOvertemperature(sensor[i]);

        if (underVoltageOccured || overVoltageOccured || overTempOccured) {
            isolateBattery();
        }
    }

    //Perform BMS functionality
    for (int i = 0; i < NUM_SENSORS; i++) {
        if (i == 0) {
            overCurrentWarning = isWarnOvercurrent(sensor[i]); //Set the kill flag if we are over current
        }
        underVoltageWarning = isWarnUndervoltage(sensor[i]);
        overVoltageWarning = isWarnOvervoltage(sensor[i]);
        overTempWarning = isWarnOvertemperature(sensor[i]);
    }

    //Calculate if we should be charging or not
    charge = !overVoltageWarning && !overCurrentWarning && !overTempWarning;
    discharge = !underVoltageWarning  && !overCurrentWarning && !overTempWarning;

    if (charge) {
        setChargingRelay(CLOSED);
    } else {
        setChargingRelay(OPEN);
    }

    if (discharge) {
        setDischargingRelay(CLOSED);
    } else {
        setDischargingRelay(OPEN);
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

void setup() {

    Serial.begin(9600);
    delay(5000);

    for (int i = 0; i < NUM_SENSORS; i++) {
        sensor[i].init();
    }

    //Timer1.initialize(POLL_PERIOD); //Intialize the hardware timer
    //Timer1.attachInterrupt(batteryCheck);  //call the poll and update method every POLL_PERIOD
    
    pinMode(PIN_RELAY_CHARGING,    OUTPUT);
    pinMode(PIN_RELAY_DISCHARGING, OUTPUT);
    pinMode(PIN_RELAY_MAIN,        OUTPUT);
    pinMode(PIN_DISP_LATCH,        OUTPUT);
    pinMode(PIN_DISP_SERIAL,       OUTPUT);
    pinMode(PIN_DISP_CLK,          OUTPUT);

    pinMode(12, OUTPUT);

    pinMode(13, OUTPUT);
    digitalWrite(13, HIGH);
    delay(3000);
    digitalWrite(13, LOW);

    digitalWrite(12, HIGH);
    delay(50);
    digitalWrite(12, LOW);
}

void loop() {
    //perform BPS functions
    for (int i = 0; i < NUM_SENSORS; i++) {
        // Grab new data for the current Sensor
        DS2776::DataUpdateStatus d = sensor[i].updateAllData();
    }
    digitalWrite(12, LOW);
    delay(200);
    // *****************  SUPER IMPORTANT - DO NOT TOUCH  *******************
    //copySensorDataToPackets();
    // **********************************************************************
}

void copySensorDataToPackets() {

    digitalWrite(13, !digitalRead(13));

    packet_s0_s3.s0_current_avg                          = sensor[MAIN_SENSOR].current_average;
    packet_s0_s3.s0_current_inst                         = sensor[MAIN_SENSOR].current_instantaneous;
    packet_s0_s3.voltage_cell0[SensorIndex::S0_MAIN]     = sensor[MAIN_SENSOR].voltage_cell_0;
    packet_s0_s3.voltage_cell1[SensorIndex::S0_MAIN]     = sensor[MAIN_SENSOR].voltage_cell_1;
    packet_s0_s3.temperature[SensorIndex::S0_MAIN]       = sensor[MAIN_SENSOR].temperature;

    for (int i = SensorIndex::S1; i < SensorIndex::S3; i++) {
        packet_s0_s3.voltage_cell0[i] = sensor[i].voltage_cell_0;
        packet_s0_s3.voltage_cell1[i] = sensor[i].voltage_cell_1;
        packet_s0_s3.temperature[i]   = sensor[i].temperature;
    }

    for (int i = SensorIndex::S4; i < SensorIndex::S7; i++) {
        packet_s4_s7.voltage_cell0[i] = sensor[i].voltage_cell_0;
        packet_s4_s7.voltage_cell1[i] = sensor[i].voltage_cell_1;
        packet_s4_s7.temperature[i]   = sensor[i].temperature;
    }

    for (int i = SensorIndex::S8; i < SensorIndex::S11; i++) {
        packet_s8_s11.voltage_cell0[i] = sensor[i].voltage_cell_0;
        packet_s8_s11.voltage_cell1[i] = sensor[i].voltage_cell_1;
        packet_s8_s11.temperature[i]   = sensor[i].temperature;
    }

    for (int i = SensorIndex::S12; i < SensorIndex::S15; i++) {
        packet_s12_s15.voltage_cell0[i] = sensor[i].voltage_cell_0;
        packet_s12_s15.voltage_cell1[i] = sensor[i].voltage_cell_1;
        packet_s12_s15.temperature[i]   = sensor[i].temperature;
    }
}

int freeRam() {
    extern int __heap_start, *__brkval;
    int v;
    return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
