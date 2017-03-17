// demo: CAN-BUS Shield, send data
#include <mcp_can.h>
#include <SPI.h>

MCP_CAN CAN(10);                                      // Set CS to pin 10
uint16_t temp = 0;

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

unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};
void loop()
{
  for(int i= 109; i<= 136; i++){
   
    stmp[0] = highByte(++temp);
    stmp[1] = lowByte(temp);
    stmp[2] = highByte(++temp);
    stmp[3] = lowByte(++temp);
    stmp[4] = highByte(++temp);
    stmp[5] = lowByte(++temp);
    stmp[6] = highByte(++temp);
    stmp[7] = lowByte(++temp);
    
    CAN.sendMsgBuf(i, 0, 8, stmp);
    delay(100);
  }
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
