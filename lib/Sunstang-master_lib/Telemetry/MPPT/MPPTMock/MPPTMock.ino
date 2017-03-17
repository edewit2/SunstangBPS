#include <mcp_can.h>
#include <SPI.h>

MCP_CAN CAN(10);                                      // Set CS to pin 10
const uint16_t REQUEST_BASE = 0x710;
const uint16_t RESPONSE_BASE = 0x770;

//Mock Data
uint8_t mpptId = B001; //From 0001 - 1111;
boolean BVLR = true;  //Battery Voltage Level Reaeched Flag
boolean OVT = false; //Over temp flag
boolean NOC = true;  //No charge flag(0, battery connected)
boolean UNDV = false; //Under Voltage

uint16_t Vin = 0x3AA;  //10 bits!
uint16_t CurrentIn = 0x355;
uint16_t Vout = 0x255;
uint8_t temp = 25;

unsigned char buff[7];

void setup()
{
  Serial.begin(115200);
  
  while(CAN_OK != CAN.begin(CAN_1000KBPS))                   //MPPT is hardwired for 125k
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");


    /*
     * set mask, set both the mask to 0x3ff so we're only looking at the ID's not data
     */
    CAN.init_Mask(0, 0, 0x3ff);                         // there are 2 mask in mcp2515, you need to set both of them
    CAN.init_Mask(1, 0, 0x3ff);
    CAN.init_Filt(0, 0, REQUEST_BASE | mpptId);         //Only recieve the request frames 
    
  //Setup up our data
    
  buff[0] = BVLR << 7 | OVT << 6 | NOC << 5 | UNDV << 5 | (0x3FF & Vin) >> 8;
  buff[1] = Vin & 0xFF;
  buff[2] = (0x3FF & CurrentIn) >> 8;
  buff[3] = CurrentIn & 0xFF;
  buff[4] = (0x3FF & Vout) >> 8;
  buff[5] = Vout & 0xFF;
  buff[6] = temp;
    
}

void loop()
{
// if(CAN_MSGAVAIL == CAN.checkReceive()){
   unsigned char len = 0;
   unsigned char buf[8];
   //CAN.readMsgBuf(&len, buf);
   CAN.sendMsgBuf(RESPONSE_BASE | mpptId, 0, 7, buff);
   buff[6]++;
   Serial.print("Response Sent");
   delay(1000);
  //}
}
