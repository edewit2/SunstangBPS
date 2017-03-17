#include <SoftwareSerial.h>


SoftwareSerial softSerial(10,11); // rx. tx

void setup() {
    Serial.begin(9600);
    softSerial.begin(9600);
    Serial.println("Hello World");
    delay(500);
}

void loop() {
    if (softSerial.available()) {
        Serial.println((byte)softSerial.read(), HEX);
    }
}
