#include <SD.h>


#include <DS1307RTC.h>
#include <Time.h>
#include <Wire.h>
#define DEBUG 2 //1: human readable
                //2: what's going to the SD
//declare the speacial bytes as consts
const byte startFlag = 0xA2;
const byte endFlag = 0x69;

//Declare the pins we use
const byte led =13;
const byte SD_CS = 10; 

File dataFile;
char buff[30];


void setup()
{
 // Open serial port
  Serial.begin(115200);
  setSyncProvider(RTC.get);  //set up the RTC
  pinMode(SD_CS, OUTPUT); //Set the Chip select up for the S
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  if(!SD.begin(SD_CS)){
    digitalWrite(led, HIGH);
}

}

void loop()
{

    while(Serial.read() != startFlag){} //block until we find a start byte
   //read the two ID's
    while(!Serial.available()){}
    uint8_t nodeIndex = Serial.read();
    while(!Serial.available()){}
    uint8_t packetIndex = Serial.read();
    while(!Serial.available()){}
    uint8_t packetLength = Serial.read();
    
    #if DEBUG == 1
      Serial.print("node: ");
      Serial.println(nodeIndex);
      Serial.print("packet: ");
      Serial.println(packetIndex);
      Serial.print("Length: ");
      Serial.println(packetLength);
    #endif
    
    if( packetLength <= 30){ //Prevent an over flow
      Serial.readBytes(buff, packetLength);
      if(Serial.read() == endFlag){  // the packet arrived well
        //make our file name
        String temp = "";
        temp.concat(nodeIndex);
        temp.concat("_");
        temp.concat(packetIndex);
        temp.concat(".dat");
 
        char filename[temp.length()+1];
        temp.toCharArray(filename, sizeof(filename));
        #if DEBUG == 1 
          Serial.print("filename");
          Serial.println(filename);
        #endif
       
        #if DEBUG == 2
          time_t t = now();
          Serial.write((uint8_t*)&t, 4);
          Serial.write((uint8_t*)buff, packetLength);
        #endif
       
        
        dataFile = SD.open(filename, FILE_WRITE); //open the file (finally)
        if(dataFile){
          time_t t = now();
          dataFile.write((uint8_t*)&t, 4);
          dataFile.write((uint8_t*)buff, packetLength);
          dataFile.close();
        }
      }
    }
}
