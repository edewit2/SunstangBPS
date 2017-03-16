#include <Wire.h>

// Global Variables
uint8_t tx_buffer[2] = {0};

void setup() {
  // put your setup code here, to run once:
  Wire.begin(16);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  
  Serial.begin(9600);           // start serial for output

  initCurrentSensor();
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t data = 0;
  input = analogRead(A0);
  tx_buffer[data++] = input >> 8;
  tx_buffer[data] = input & 0xFF;
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write(tx_buffer,2);
}

void initCurrentSensor(void) {
  analogReference(DEFAULT);
  analogRead(A0);
}

