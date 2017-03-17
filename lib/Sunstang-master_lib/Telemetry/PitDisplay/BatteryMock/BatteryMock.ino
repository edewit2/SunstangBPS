void setup() {
    //This can be whatever.  
     Serial.begin(9600);
}

  int cellNum = 0;
  float voltage = 0;
void loop() {
  voltage = float(random(33, 50))/10;
  Serial.print((cellNum++)%32);
  Serial.print(" ");
  Serial.println(voltage);
  delay(50);               // wait for a bit
}
