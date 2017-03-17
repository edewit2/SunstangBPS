int accelerator=A5;
int brake=1;

void setup()
{
  pinMode(brake,INPUT);
}

void loop()
{
  int acceleratorval=analogRead(accelerator);
  uint8_t canBuffer[8]; //unsigned 8-bit integer
  canBuffer[0] = highByte(acceleratorval);
  canBuffer[1] = lowByte(acceleratorval);
  canBuffer[2] = digitalRead(brake)
  /*
  1024 = 2^10 --> meaning we need AT LEAST 10 bits to represent this number
  But, in arduino, the minimum data size is 8 bits, or 1 byte.
  so, if we need at least 10 bits, then we need at least 2 bytes to represent this number.
  This means, we need at least 2 bytes, or 16 bits.
  1024 in Binary is:  00000010 00000000 -> 16 bits, but we only use 10.
                        high     low
  */
  // to reassemble the original number (16 bit unsigned integer, or int)
  // int receivingEnd = word(/*highbyte*/ canBuffer[0], /*lowByte*/ canBuffer[1]);
 demo: CAN-BUS Shield, send data
#include <mcp_can.h>
#include <SPI.h>

MCP_CAN CAN(10);                                      // Set CS to pin 10

unsigned char Flag_Recv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];

unsigned long timeNow, timeLast;

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
	timeLast = millis();
}

//this is the data we want to write over the can bus.
unsigned char stmp[8] = {0, 1, 2, 3, 4, 5, 6, 7};


void loop()
{
  // analog read, for the pot. measurement
  // put the highbyte and low byte into the stmp (canBuffer) array in position 0 and 1
  // and the remaining code below will handle the rest.
    // send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
	timeNow = millis();
	if (timeNow - timeLast > 1000) {
		CAN.sendMsgBuf(0x00, 0, 8, stmp);
		timeLast = timeNow;
	}

	if (CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
	{
		CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf

		for (int i = 0; i<len; i++)    // print the data
		{
			byte b = buf[i];
			if (b < 9) {
				Serial.print(b); Serial.print("\t");
			} else {
				Serial.print((char)b); Serial.print("\t");
			}
		}
		Serial.println();
		Serial.println();
	}
}

/*********************************************************************************************************
  END FILE
*********************
}
