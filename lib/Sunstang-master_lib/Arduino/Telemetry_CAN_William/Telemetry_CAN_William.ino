// demo: CAN-BUS Shield, send data
#include <mcp_can.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

MCP_CAN CAN(10);                                      // Set CS to pin 10

TinyGPS gps;
SoftwareSerial nss(3, 4);

char input;

//static void gpsdump(TinyGPS &gps);
static bool feedgps();

unsigned char Flag_Recv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];

unsigned long timeNow, timeLast;

long lat, lon, alt;//latitude, longitude, altitude
unsigned long time, vel, dir;//time, velocity, course(direction)

unsigned char stmp[4][8] = { { 0, 1, 2, 3, 4, 5, 6, 7 },
{ 0, 1, 2, 3, 4, 5, 6, 7 },
{ 0, 1, 2, 3, 4, 5, 6, 7 },
{ 0, 1, 2, 3, 4, 5, 6, 7 } };

void setup()
{

	//Initializing things for the CAN bus####################
	Serial.begin(115200);

START_INIT:

	if (CAN_OK == CAN.begin(CAN_500KBPS))                   // init can bus : baudrate = 500k
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

	//Initializing things for the gps#######################
	nss.begin(9600);
}

//MAIN LOOP##################################################
void loop()
{
	//Getting GPS Data
	bool newdata = false;
	unsigned long start = millis();

	// Every second we print an update
	timeNow = millis();

	if (timeNow - timeLast < 1000)
	{//feedgps while we water for the second-update
		if (feedgps())
			newdata = true;//not entirely necessary
	}
	// send data:  id = 0x00, standrad flame, data len = 8, stmp: data buf
	else /*(timeNow - timeLast > 1000)*/ {
		//Getting the data from the gps
		gps.get_position(&lat, &lon, &time);
		alt = gps.altitude();
		vel = gps.f_speed_mps();
		dir = gps.course();
                
                Serial.print("lat: ");
                Serial.println(lat);
                
                Serial.print("lon: ");
                Serial.println(lon);
                
                Serial.print("time: ");
                Serial.println(time);
                
                Serial.print("alt: ");
                Serial.println(alt);
                
                Serial.print("vel: ");
                Serial.println(vel);
                
                Serial.print("dir: ");
                Serial.println(dir);
                
		//Formatting the messages for transmission
		//stmp[0][0] = ((lat >> 24) & 0xFF) //this thing, but more
		///msg 1
		for (int i = 0; i < 4; i++){
			stmp[0][i] = ((lat >> (8 * (3 - i))) & 0xFF);
		}
		for (int i = 0; i < 4; i++){
			stmp[0][i + 4] = ((lon >> (8 * (3 - i))) & 0xFF);
		}
		///msg 2
		for (int i = 0; i < 4; i++){
			stmp[1][i] = ((time >> (8 * (3 - i))) & 0xFF);
		}
		for (int i = 0; i < 4; i++){
			stmp[1][i + 4] = ((vel >> (8 * (3 - i))) & 0xFF);
		}
		///msg 3
		for (int i = 0; i < 4; i++){
			stmp[2][i] = ((time >> (8 * (3 - i))) & 0xFF);
		}
		for (int i = 0; i < 4; i++){
			stmp[2][i + 4] = ((dir >> (8 * (3 - i))) & 0xFF);
		}


                for(int i = 0; i < 3; i ++){
                  Serial.println();
                  for(int j = 0; j < 8; j ++){
                    Serial.print(stmp[i][j]);
                    Serial.print(" ");
                  }
                }
                Serial.println();
                
		CAN.sendMsgBuf(0xAAA0, 0, 8, stmp[0]);
		CAN.sendMsgBuf(0xAAA1, 0, 8, stmp[1]);
		CAN.sendMsgBuf(0xAAA2, 0, 8, stmp[2]);
		//CAN.sendMsgBuf(0xAAA3, 0, 8, stmp[3]);
		timeLast = timeNow;
	}

	//if (CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
	//{
	//	CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
        //
	//	for (int i = 0; i<len; i++)    // print the data
	//	{
	//		byte b = buf[i];
	//		if (b < 9) {
	//			Serial.print(b); Serial.print("\t");
	//		}
	//		else {
	//			Serial.print((char)b); Serial.print("\t");
	//		}
	//	}
	//	Serial.println();
	//	Serial.println();
	//}
}

/**********************************************************************************************************
END FILE
*********************************************************************************************************/

static bool feedgps()
{
	while (nss.available())
	{
		input = nss.read();

		//Serial.print(input);

		if (gps.encode(input))
			return true;
	}
	return false;
}
