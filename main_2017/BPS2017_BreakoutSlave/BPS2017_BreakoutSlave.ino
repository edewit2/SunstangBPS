const uint8_t statusLed1 = 2;
const uint8_t statusLed2 = 3;
const uint8_t statusLed3 = 4;
const uint8_t statusLed4 = 5;

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  Serial.begin(9600);           // start serial for output
  pinMode(statusLed1,OUTPUT);
  pinMode(statusLed2,OUTPUT);
  pinMode(statusLed3,OUTPUT);
  pinMode(statusLed4,OUTPUT);
}

void loop() {
    digitalWrite(statusLed1, bitRead(count,0));
    digitalWrite(statusLed2, bitRead(count,1));
    digitalWrite(statusLed3, bitRead(count,2));
    digitalWrite(statusLed4, bitRead(count,3));   
}

void receiveEvent(int howMany) {
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}
