#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Time.h>
#include <mcp_can.h>
#include <SPI.h>
#include <CANHelper.h>
#include <GPSHelper.h>

#define RX_PIN 3
#define TX_PIN 4
#define GPS_BAUD 9600
#define DEBUG_BAUD 115200

#define DEBUG 0

// The TinyGPS++ object
TinyGPSPlus gps;

//CAN bus object
MCP_CAN CAN(10);

// The serial connection to the GPS device
SoftwareSerial ss(RX_PIN, TX_PIN);

void setup() {
  ss.begin(GPS_BAUD);
  #if DEBUG == 1
  Serial.begin(DEBUG_BAUD);
  Serial.println("Software Serial (GPS) initialized");
  #endif
  
  while(CAN.begin(CAN_SPEED) != CAN_OK)
  {
    #if DEBUG == 1
    Serial.println("CAN BUS Shield init fail");
    Serial.println("Init CAN BUS Shield again");
    #endif
    delay(100);
  }
  #if DEBUG == 1
  Serial.println("CAN BUS Shield initialized");
  #endif
}

void loop() {
  while (ss.available() > 0)    //Wait for GPS to send data
  {
    if (gps.encode(ss.read())) //Read the data and give it to the parser
    {
      //Speed and course.
      //These come in the same nmea package from the GPS and there in the same CAN msg so we deal with together
      if((gps.speed.isUpdated() || gps.course.isUpdated()) && gps.speed.isValid() && gps.course.isValid())
      {
        uint8_t speedCourseBuffer[8];
        uint32_t speed = gps.speed.mps()*100;  //CM/s
         
        int32_t direction = gps.course.value();  //100th of a degree
        
        insert32Bit(0, speedCourseBuffer, speed);
        insert32Bit(4, speedCourseBuffer, direction);
         
        CAN.sendMsgBuf(GPS_SPEED_COURSE_CAN_ID, 0, 8, speedCourseBuffer);
        
        #if DEBUG == 1
        Serial.print("Direction:  ");
        Serial.println(direction);
        Serial.print("Speed: ");
        Serial.println(speed); //cm/s
        #endif
      }
           
      //Location
      if(gps.location.isUpdated() && gps.location.isValid())
      {
        uint8_t locationBuffer[8];
        int32_t latitude = gps.location.lat() * 1e6;
        int32_t longitude = gps.location.lng() * 1e6;
        
        #if DEBUG == 1
        Serial.print("latitude: ");
        Serial.println(latitude);
        Serial.print("longitude: ");
        Serial.println(longitude);
        #endif
        
        insert32Bit(0, locationBuffer, latitude);
        insert32Bit(4, locationBuffer, longitude);
         
        CAN.sendMsgBuf(GPS_LOCATION_CAN_ID, 0, 8, locationBuffer);
      }
       
      //Time and Altitude
      if((gps.time.isUpdated() || gps.altitude.isUpdated()) && gps.altitude.isValid() && gps.time.isValid() && gps.date.isValid())
      {
        uint8_t timeAltBuffer[8];
        TimeElements currentTime;
        currentTime.Second = gps.time.second();  //Seconds   0 to 59
        currentTime.Minute = gps.time.minute();  //Minutes   0 to 59
        currentTime.Hour =   gps.time.hour();    //Hours     0 to 23
        currentTime.Day = gps.date.day();        //Day       1 to 31
        currentTime.Month = gps.date.month();    //Month     1 to 12
        
        //gps.date.year gives us the year eg(2015), we need it from 1970
        currentTime.Year  = gps.date.year()-1970; //Year      0 to 99 (offset from 1970)
        
        uint32_t time = makeTime(currentTime);
        
        int32_t altitude = gps.altitude.value();
        
        insert32Bit(0, timeAltBuffer, altitude);
        insert32Bit(4, timeAltBuffer, time);
         
        CAN.sendMsgBuf(GPS_ALTITUDE_TIME_CAN_ID, 0, 8, timeAltBuffer);
        
        #if DEBUG == 1
        Serial.print("Time: ");
        Serial.println(time);
        Serial.print("Alitude: ");
        Serial.println(altitude);
        #endif
      } 
    }
  }
}
