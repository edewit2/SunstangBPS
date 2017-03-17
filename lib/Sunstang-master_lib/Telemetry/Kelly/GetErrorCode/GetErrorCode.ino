// demo: CAN-BUS Shield, send data
#include <mcp_can.h>
#include <SPI.h>
#include <KellyHelper.h>

MCP_CAN CAN(10);                                      // Set CS to pin 10

void setup()
{
    Serial.begin(115200);

START_INIT:

    if(CAN_OK == CAN.begin(CAN_1000KBPS))                   // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init ok!");
    }
    else
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println("Init CAN BUS Shield again");
        delay(100);
        goto START_INIT;
    }
}

void loop()
{
    CAN.sendMsgBuf(KELLY_REQUEST, 0, CCP_MONITOR2_REQUEST.len, CCP_MONITOR2_REQUEST.data);
    
     while(CAN_MSGAVAIL != CAN.checkReceive()){}           // check if data coming
     uint8_t len;
     uint8_t buff[8];
     CAN.readMsgBuf(&len, buff);    // read data,  len: data length, buf: data buf

     if(len == 5)
     {
       if(buff[3] & 0x80){
         //44
         Serial.println("Uknown error: 0x44");
       }
       
       if(buff[3] & 0x40){
         //43
         Serial.println("Motor Over Temp: 0x43");
       }
       
       if(buff[3] & 0x20){
         //42
         Serial.println("Hall sensor error: 0x42");
       } 
       
       if(buff[3] & 0x10){
         //41
         Serial.println("Regen or Startup over voltage: 0x41");
       }
       
       if(buff[3] & 0x08){
         //34
         Serial.println("Non-zero throttlo on direction change: 0x31");
       }
       
       if(buff[3] & 0x04){
         //33
         Serial.println("Hall throttle is open or short circuit: 0x33");
       }
       
       if(buff[3] & 0x02){
         //32
         Serial.println("Internal reset: 0x32");
       } 
       
       if(buff[3] & 0x01){
         //31
         Serial.println("Frequent error: 0x31");
       } 
       
       if(buff[4] & 0x80){
         //24
         Serial.println("Throttle Error at power up: 0x24");
       }
       
       if(buff[4] & 0x40){
         //23
         Serial.println("Controller over temperature: 0x23");
       }
       
       if(buff[4] & 0x20){
         //22
         Serial.println("Interal voltage fault: 0x22");
       } 
       
       if(buff[4] & 0x10){
         //21
         Serial.println("Motor Did not start: 0x21");
       }
       
       if(buff[4] & 0x08){
         //14
         Serial.println("Controller Over Temp warning: 0x14");
       }
       
       if(buff[4] & 0x04){
         //13
         Serial.println("Low voltage error(on high power): 0x13");
       }
       
       if(buff[4] & 0x02){
         //12
         Serial.println("Over voltage error on high power side: 0x12");
       } 
       
       if(buff[4] & 0x01){
         //11
         Serial.println("Uknown error: 0x11");
       }       
     }

     Serial.println();
     delay(1000);
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
