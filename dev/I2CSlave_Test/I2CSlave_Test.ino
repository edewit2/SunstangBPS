#include <Wire.h>

// Digital Pinout

// Analog Pinout
const uint8_t analogPin = 0;

// Global Variables
const uint8_t Nsensors = 1; // number of current sensors
uint8_t tx_buffer[Nsensors] = {0};  // send one byte per current sensor device
float smoothCurrent = 0;  // exponentially smoothed current
const float alpha = 0.8;  // exponential smoothing parameter (0 < alpha < 1)

void setup() { 
  Serial.begin(9600);           // start serial for output  
  initCurrentSensor();
  Wire.begin(16);               // join i2c bus with address #16
  Wire.onRequest(requestEvent); // register event
}

void loop() {
    int ADCval = analogRead(analogPin);
    float ADCvoltage = ADCval/1023.0*5.0;
    float current = toAmperes(ADCvoltage);
    smoothCurrent = alpha*current + (1 - alpha)*smoothCurrent;
    tx_buffer[0] = (uint8_t)(smoothCurrent*2.8);
    Serial.print("ADC result: ");Serial.println(ADCval);
    Serial.print("ADC voltage: ");Serial.println(ADCvoltage);
    Serial.print("Current: ");Serial.println(smoothCurrent);
    Serial.print("I2C data: ");Serial.println(tx_buffer[0]);
    Serial.println();
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write(tx_buffer,Nsensors);
}

void initCurrentSensor(void) {
  
}

float toAmperes(float adcVoltage) {
  return (24.5205*adcVoltage - 31.2500);
}

