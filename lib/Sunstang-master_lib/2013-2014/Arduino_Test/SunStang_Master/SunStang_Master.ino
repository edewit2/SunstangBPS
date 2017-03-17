#include <Packets_Test.h>
#include <I2cNetworkSlave.h>
#include <TimedFunction.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SlaveNode.h>
#include <Packet.h>
#include <I2cNetworkMaster.h>

#include <Packets_S1.h>
#include <Packets_S2.h>
#include <Packets_S3.h>

#define CARRIAGE_RETURN 13
#define START_BYTE 0xA2
#define END_BYTE 0x69

SlaveNode node[] = {Packet_S1::slaveNode, Packet_S2::slaveNode, Packet_S3::slaveNode};
//SlaveNode node[] = {Packet_S1::slaveNode, Packet_S2::slaveNode};
I2cNetworkMaster master(&Wire, node, sizeof(node) / sizeof(node[0]));

struct Request {
	uint8_t address;
	uint8_t packetId;
};

Request requestTable[] = {
	{0, 0}, // address, packet ID
	{0, 1},
	{1, 0},
	{1, 1},
	{2, 0},
	{2, 1},
	{2, 2}
};
uint32_t now = 0, n = 0, then = 0, update = 10;

long avgWaitTime = 0;

void setup() {
	Serial.begin(115200);
	Wire.begin();
	pinMode(13, OUTPUT);
	Serial.print("Free RAM: ");
	Serial.println(freeRam());
	establishContact();
	then = millis();


	avgWaitTime = averageWaitTime();
}

int index = 0;
int maxIndex = 6;

void loop() {
	//ratioDropped();
	now = millis();
	if (index > maxIndex) {
		index = 0;
	}
	//n += master.obtainPacket(node[requestTable[index].address].address,
	//						 node[requestTable[index].address].
	//						 packet[requestTable[index].address].id);
	n += master.obtainPacket(requestTable[index].address, requestTable[index].packetId);
	//index++;
	if (now - then > update) {
		digitalWrite(13, !digitalRead(13));
		then = now;
		//transmitPacket(0, 0);
		printPackets(index++);
		/*for (int i = 0; i <= maxIndex; i++) {
			printPackets(i);
		}*/
		//index++;
		//printPackets(0);
		//printPackets(1);
		Serial.println(n);
	//	Serial.println();
		//delay(1000);
	}
	//delay(1);
	//Serial.println("--------------------------");
}

long averageWaitTime() {
	long avgWaitTime = 0;
	for (int i = 0; i < 100; i++) {
		uint32_t t1 = millis();
		for (int i = 0; i < 100; i++) {
			master.obtainPacket(0, 0);
		}
		uint32_t t2 = millis() - t1;
		avgWaitTime += t2;
		Serial.print("avgWaitTime: ");
		Serial.println(t2);
	}
	Serial.print("avgWaitTime: ");
	Serial.println(avgWaitTime/100);
	return avgWaitTime / 100;
}

void ratioDropped() {
	static int lag = 0;
	static int noLag = 0;
	uint32_t t1 = millis();
	for (int i = 0; i < 100; i++) {
		master.obtainPacket(0, 0);
	}
	long t2 = millis() - t1;
	if (abs(t2 - avgWaitTime) > 100) {
		lag++;
		Serial.print("over: ");
		Serial.print(t2);
		Serial.print(" - ");
		Serial.print(avgWaitTime);
		Serial.print(" = ");
		Serial.println(t2-avgWaitTime);
		Serial.print("lag:noLag, ");
		Serial.print(lag); Serial.print(":");
		Serial.println(noLag);
	} else {
		noLag++;
	}
}

void transmitPacket(uint8_t nodeIndex, uint8_t packetId) {
	uint8_t  packetSize = node[nodeIndex].packet[packetId].size;
	uint8_t* pReadByte = node[0].packet[packetId].pStruct;
	//Serial.println(packetSize);
	Serial.write(START_BYTE);
	Serial.write(nodeIndex);
	Serial.write(packetId);
	Serial.write(packetSize);
	Serial.write(pReadByte, packetSize);
	Serial.write(END_BYTE);
}

void establishContact() {
	while (Serial.available() <= 0) {
		Serial.print('A');   // send a capital A
		Serial.print('\n');
		delay(1000);
		then = millis();
		digitalWrite(13, HIGH);
	}
	Serial.println("Received start message!");
}

void printPackets(int i) {
	switch (i) {
		case 0:
			Serial.print("S1P0D0: ");
			Serial.println(Packet_S1::packet0.data0);
			break;
		case 1:
			Serial.print("S1P1D1: ");
			Serial.println(Packet_S1::packet1.data1);
			Serial.print("S1P1D1: ");
			Serial.println(Packet_S1::packet1.data2);
			break;
		case 2:
			Serial.print("S2P0D0: ");
			Serial.println(Packet_S2::packet0.data0);
			break;
		case 3:
			Serial.print("S2P1D1: ");
			Serial.println(Packet_S2::packet1.data1);
			Serial.print("S2P1D2: ");
			Serial.println(Packet_S2::packet1.data2);
			Serial.print("S2P1D3: ");
			Serial.println(Packet_S2::packet1.data3);
			break;
		case 4:
			Serial.print("S3P0D0[0]: ");
			Serial.println(Packet_S3::packet0.data0[0]);
			Serial.print("S3P0D0[1]: ");
			Serial.println(Packet_S3::packet0.data0[1]);
			Serial.print("S3P0D0[2]: ");
			Serial.println(Packet_S3::packet0.data0[2]);
			Serial.print("S3P0D0[3]: ");
			Serial.println(Packet_S3::packet0.data0[3]);
			Serial.print("S3P0D0[4]: ");
			Serial.println(Packet_S3::packet0.data0[4]);
			Serial.print("S3P0D0[5]: ");
			Serial.println(Packet_S3::packet0.data0[5]);
			break;
		case 5:
			Serial.print("S3P1D1: ");
			Serial.println(Packet_S3::packet1.data1);
			Serial.print("S3P1D2: ");
			Serial.println(Packet_S3::packet1.data2);
			break;
		case 6:
			Serial.print("S3P2D3[0]: ");
			Serial.println(Packet_S3::packet2.data3[0]);
			Serial.print("S3P2D3[1]: ");
			Serial.println(Packet_S3::packet2.data3[1]);
			Serial.print("S3P2D3[2]: ");
			Serial.println(Packet_S3::packet2.data3[2]);
			break;
	}
}

int freeRam() {
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
