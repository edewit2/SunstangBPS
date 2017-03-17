// demo: CAN-BUS Shield, send data
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
*********************************************************************************************************/
