// Sunstang Battery Protection System 2015: Temperature Control 
// This controller reads one analog voltage selected from 32 temperature sensors using four 4051 analog multiplexers.
// Isolates the battery if overtermperature is detected. Must reset to re-enable battery.
// Sends data via CANbus to both the driver display and telemetry

// Address lines for multiplexers
const byte PIN_ADDRESS_OUTPUT_A = 0;      // Address lines of all four multiplexers are tied
const byte PIN_ADDRESS_OUTPUT_B = 1;
const byte PIN_ADDRESS_OUTPUT_C = 2;
const byte PIN_CHIP_INHIBIT_OUTPUT_1 = 3;
const byte PIN_CHIP_INHIBIT_OUTPUT_2 = 4;
const byte PIN_CHIP_INHIBIT_OUTPUT_3 = 5;
const byte PIN_CHIP_INHIBIT_OUTPUT_4 = 6;

// The input of the selected sensor
const byte PIN_TEMPERATURE_SENSOR_INPUT = 8;

// Indicator for weather or not the batteries are charging. This will come from the BPS current-controller.
const byte PIN_CURRENT_DIRECTION_INDICATOR = 7;

// Simultaneously controls both the charging and the discharging relays
const byte PIN_RELAY_OUTPUT = 9;

const float    TEMPERATURE_PER_INT16 = x;      // Degrees Celcius/int

const int16_t  ABSOLUTE_OVER_TEMPERATURE_CHARGING    = 45; // Threshold: 45 Celcius (Units: x Celcius/integer)
const int16_t  ABSOLUTE_OVER_TEMPERATURE_DISCHARGING = 60; // Threshold: 60 Celcius (Units: x Celcius/integer)


void setup()
{

}

void loop()
{
  // Run check for each of the 32 sensors
  for (byte chip = 0; chip <= 3; chip++)
  {
    for (byte address = 0; address < 8; address++)
    {
      // Select the correct input
      
      // Read the analog input
      
      // Shut off relay if sensor is overtemperature
      
      // Send data to telemetry
    }
  }
  
  // Send data to driver display
  
}

boolean isOvertemperature()
{

}

void isolateBattery()
{

}
