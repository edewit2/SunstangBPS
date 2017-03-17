#include <mcp_can.h>
#include <SPI.h>
#include <CANHelper.h>

MCP_CAN CAN(10);

void setup() {
  
  Serial.begin(115200);
  
  while(CAN.begin(CAN_SPEED) != CAN_OK)
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println("Init CAN BUS Shield again");
    delay(100);
  }
  
  //Since we only care about a few packets only ask for those.
  CAN.init_Mask(0, 0, 0x7F0);                         // there are 2 mask in mcp2515, you need to set both of them
  CAN.init_Mask(1, 0, 0x7F0);
  
  CAN.init_Filt(0, 0, 0x060);                          // there are 6 filter in mcp2515
  CAN.init_Filt(1, 0, 0x160);
  CAN.init_Filt(2, 0, 0x770);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint32_t id;
  uint8_t buff[8];
  uint8_t len;
  if(CAN_MSGAVAIL == CAN.checkReceive()){
    CAN.readMsgBufID(&id, &len, buff); 
    Serial.println(id);  
  }
    delay(100);
}
