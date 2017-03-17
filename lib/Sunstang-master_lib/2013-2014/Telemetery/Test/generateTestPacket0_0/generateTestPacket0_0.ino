void setup()
{
 Serial.begin(9600);

}

void loop()
{
   Serial.write(0xA2);  //start
   Serial.write(0); //Subsys
   Serial.write(0); //pack
   Serial.write(4); //bigdata
   Serial.write(2);
   Serial.write(4);//age
   Serial.write(17);//OtherData
   Serial.write(0xFF);
   Serial.write(0x69);
  delay(100);
}
