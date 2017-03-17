
int pinV1 = 4;
int pinV2 = 3;
int pinCurrent = 1;
int pinTemperature = 2;

void sendResponse(byte echoCommand, byte response[], int responseLength){
  
   // Beginning of response
   Serial.write(0xCD);
   Serial.write(echoCommand);
   for (int i = 0; i < responseLength; ++i)
   {
      Serial.write(response[i]);
   }
   // End of responses
   Serial.write(0xCD);
}
unsigned int scaleVoltageOrTemp(int sensorReading){
   // input ranges from 0 to 1023
   // output ranges from -1024 to 1023
   return ((sensorReading * 2) << 5) ^ 0x8000;
}
unsigned int scaleCurrent(int sensorReading){
   // input ranges from 0 to 1023
   // output ranges from -32768 to 32767
   return (sensorReading * 64) ^ 0x8000;
}

void loop() 
{
     
   static int count = 0;
   if(Serial.available()){
      
      if (Serial.read() == 0x69) {
         count++;
      }
      if (count >= 2) {
         // Reat sensor values
         int v1Reading = analogRead(pinV1);
         int v2Reading = analogRead(pinV2);
         int currentReading = analogRead(pinCurrent);
         int temperatureReading = analogRead(pinTemperature);
         // Scale to a 16 bit number
         int outputV1 = scaleVoltageOrTemp(v1Reading);
         int outputV2 = scaleVoltageOrTemp(v2Reading);
         int outputCurrent = scaleCurrent(currentReading);
         int outputTemperature = scaleVoltageOrTemp(temperatureReading);
         // Create 
         byte response[] = {
            0xFF,    
            0xFF,
            highByte(outputTemperature), 
            lowByte(outputTemperature),
            highByte(outputV1),
            lowByte(outputV1),
            highByte(outputCurrent),
            lowByte(outputCurrent),
            0xFF,   
            0xFF,
            0xFF,   
            0xFF,
            0xFF,
            0xFF,
            0xFF,   
            0xFF,
            0xFF,   
            0xFF,
            0xFF,   
            0xFF,
            highByte(outputV2),
            lowByte(outputV2)          
         };
         sendResponse(0x69,response, 22);
         count = 0;
      }  
   }
}

void setup() 
{                
   Serial.begin(9600);     
}
