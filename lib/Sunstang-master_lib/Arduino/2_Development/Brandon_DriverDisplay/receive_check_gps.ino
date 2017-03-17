// demo: CAN-BUS Shield, receive data with check mode
// send data coming to fast, such as less than 10ms, you can use this way
// loovee, 2014-6-13


#include <SPI.h>
#include <LiquidCrystal.h>
#include "mcp_can.h"

LiquidCrystal lcd(9, 8, 5, 4, 3, 2);

unsigned char Flag_Recv = 0;
unsigned char len = 0;
unsigned char buf[8], lat[8], dir[8], vel[8];
char str[20];
unsigned long variable1(0), variable2(0);

MCP_CAN CAN(10);                                            // Set CS to pin 10

void setup()
{
    Serial.begin(115200);

START_INIT:

    if(CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
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
    
    lcd.begin(16, 2);
}


void loop()
{
    lcd.setCursor(0,0);
    variable2 = 0;
    if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
        
        for(int i(0); i<4; i++)
        {
          variable2+= (buf[i]<<(8*(3-i)));    
        }
        
        Serial.print(variable2);
        Serial.println();
        variable2 = 0;
     
        for(int i(4); i<8; i++)
        {
          variable2 += (buf[i]<<(8*(3-i+4))); 
        } 
        Serial.print(variable2);
        Serial.println();   
    }
    //lcd.print("I'm working! :D");
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
