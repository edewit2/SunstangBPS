#include<Arduino.h>
#include <CANHelper.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  uint8_t t[8];
  insert32Bit(0, t, (uint32_t) 0x01020304);
  insert32Bit(4, t, (uint32_t) 0x21222324);
  
  if(t[0] != 0x01 || t[1] != 0x02 || t[2] != 0x03 || t[3] != 0x04 || t[4] != 0x21 || t[5] != 0x22 || t[6] != 0x23 || t[7] != 0x24)
     {
       Serial.println("insert32Bit is broken"); 
     }
  
  if(get32BitUint(0, t) != 0x01020304 || get32BitUint(4, t) != 0x21222324)
  {
    Serial.println("get32BitUint is broken"); 
  }
  
  Serial.println("Done!");
  delay(2000);
}
