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
}

uint16_t ids[] = {100,
101,
102,
103,
104,
105,
106,
107,
108,
109,
110,
111,
112,
113,
114,
115,
116,
117,
118,
119,
120,
121,
122,
123,
124,
125,
126,
127,
128,
129,
130,
131,
132,
133,
134,
135,
136,
137,
138,
192,
193,
194,
195,
196,
197,
198,
352,
353,
672,
673,
674,
1809,
1810,
1905,
1906,
};

void loop() {
  // put your main code here, to run repeatedly:

for(int i =0; i < 55; i++){
  CAN.sendMsgBuf(ids[i], 0, 0, 0);
  Serial.println(ids[i]);
  delay(100);
}
}
