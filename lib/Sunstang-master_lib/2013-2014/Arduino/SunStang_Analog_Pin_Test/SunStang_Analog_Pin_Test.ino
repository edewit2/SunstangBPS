const uint8_t pwmOutPin   = 0;
const uint8_t analogInPin = 5;

void setup() {
	Serial.begin(115200);
	Serial.println("Init");
	analogWrite(0, (int)((float)(0.8)*255));
	Serial.print("Expecting 4V, at: ");
	Serial.println((int)((float)(0.8) * 1023));
	Serial.println("Device Ready!");
}

void loop() {
	Serial.println(analogRead(analogInPin));
}
