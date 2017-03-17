// Sunstang Battery Protection System 2015: Current Control 
// This controller reads an analog voltage from a current sensing circuit. 
// Isolates battery by disabling relay if overcurrent is detected. Must reset to re-enable current.
// Sends data via CANbus to both the driver display and telemetry

// The input of the current sensor
const byte PIN_CURRENT_SENSOR_INPUT = 8;

// Indicator for weather or not the batteries are charging. current-sensing circuit.
const byte PIN_CURRENT_DIRECTION_INDICATOR = 7;

// Simultaneously controls both the charging and the discharging relays
const byte PIN_RELAY_OUTPUT = 9;

const float    TEMPERATURE_PER_INT16 = x;      // Degrees Celcius/int
const float    R_SNS                 = x;      // Sense Resistance (ohms)

const int16_t  ABSOLUTE_OVER_CURRENT_CHARGING    = x; 
const int16_t  ABSOLUTE_OVER_CURRENT_DISCHARGING = x; 


void setup()
{

}

void loop()
{
  // Sample current
  
  // Shut off relay if sensor indicates overcurrent
  
  // Send data to driver display
  
  // Send data to telemetry
}

boolean isOvercurrent()
{

}

void isolateBattery()
{

}
