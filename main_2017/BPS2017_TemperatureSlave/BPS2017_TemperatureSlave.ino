// BPS 2017 Thermistor Measurement Board
// This sketch uses a parallel addreses to communicate with a multiplexer which selects a thermistor input.
// This variable resistance of the thermistor is converted to a voltage using a voltage divider with a 10k resistor.
// The voltage of the divider is measured using the ADC. The digital code is first converted to celcius as a float variable.
// The value in celcius is then multiplied by 4 and cast into a single byte for simple transmisssion accross the I2C interface.
// Last modified: January 31, 2017

#include <Wire.h>

// Digital Pinout
const uint8_t MUX_EN = 4;
const uint8_t MUX_S0 = 5;
const uint8_t MUX_S1 = 6;
const uint8_t MUX_S2 = 7;
const uint8_t MUX_S3 = 8;

// Analog Pinout
const uint8_t analogPin = 0;

// Global Variables
const uint8_t Nsensors = 8; // number of thermistors
uint8_t tx_buffer[Nsensors] = {0};  // send one byte per thermistor

void setup() { 
  Serial.begin(9600);           // start serial for output  
  initTemperatureSensor();
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
}

void loop() {
  for(uint8_t sensorAddress = 0; sensorAddress<Nsensors; sensorAddress++){
    writeAddress(sensorAddress);
    delay(1);   // multiplexer switching delay
    int ADCval = analogRead(analogPin);
    float ADCvoltage = ADCval/1023.0*5.0;
    float temperature = toCelcius(ADCvoltage);
    tx_buffer[sensorAddress] = (uint8_t)(temperature*4);
    Serial.print("Sensor address: ");Serial.println(sensorAddress);
    Serial.print("ADC result: ");Serial.println(ADCval);
    Serial.print("ADC voltage: ");Serial.println(ADCvoltage);
    Serial.print("Temperature: ");Serial.println(temperature);
    Serial.print("I2C data: ");Serial.println(tx_buffer[sensorAddress]);
    Serial.println();
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write(tx_buffer,Nsensors);
}

void initTemperatureSensor(void) {
  pinMode(MUX_EN, OUTPUT); 
  pinMode(MUX_S0, OUTPUT); 
  pinMode(MUX_S1, OUTPUT); 
  pinMode(MUX_S2, OUTPUT); 
  pinMode(MUX_S3, OUTPUT);  
  digitalWrite(MUX_EN, HIGH); 
  digitalWrite(MUX_S0, LOW);
  digitalWrite(MUX_S1, LOW);
  digitalWrite(MUX_S2, LOW);
  digitalWrite(MUX_S3, LOW);
}

void writeAddress(uint8_t currentAddress) {
  digitalWrite(MUX_S0, bitRead(currentAddress,0));
  digitalWrite(MUX_S1, bitRead(currentAddress,1));
  digitalWrite(MUX_S2, bitRead(currentAddress,2));
  digitalWrite(MUX_S3, bitRead(currentAddress,3));  
}

float toCelcius(float adcVoltage) {
  return (adcVoltage - 3.5758)/(-0.0405); // from matlab curve fitting
}

