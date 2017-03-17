// demo: CAN-BUS Shield, receive data with interrupt mode
// when in interrupt mode, the data coming can't be too fast, must >20ms, or else you can use check mode
// loovee, 2014-6-13

#include <SPI.h>
#include "mcp_can.h"
#include <KellyHelper.h>

MCP_CAN CAN(10);                                            // Set CS to pin 10


bool Flag_Recv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];
unsigned char stmp[] = {CCP_A2D_BATCH_READ2};
unsigned long reqTime;
unsigned long resTime;

void setup()
{
    Serial.begin(115200);

START_INIT:

    if(CAN_OK == CAN.begin(CAN_1000KBPS))         
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
    
    attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt
}

void MCP2515_ISR()
{
     resTime = micros();
     Flag_Recv = 1;
}

void loop()
{
    reqTime = micros();
    CAN.sendMsgBuf(KELLY_REQUEST, 0, 0, stmp);
    while(!Flag_Recv){}
    Flag_Recv = 0;                // clear flag
    CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
    
    if(len == 1 && buf[0] == CCP_INVALID_COMMAND)
      Serial.println("Error");
    else
      Serial.println(resTime-reqTime);
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
