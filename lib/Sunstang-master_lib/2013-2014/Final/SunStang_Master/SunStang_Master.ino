#include <TestFrame.h>
#include <FrameIO.h>
#include <FrameInterface.h>
#include <FrameFactory.h>
#include <DriverInterfaceFrame.h>

#include <TimedFunction.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SlaveNode.h>
#include <Packet.h>
#include <I2cNetworkMaster.h>

#include <Packets_S0.h>
#include <Packets_S1.h>
#include <Packets_S2.h>

#define CARRIAGE_RETURN 13

SlaveNode node[] = {Packet_S0::slaveNode, Packet_S1::slaveNode, Packet_S2::slaveNode};
uint8_t numNodes = sizeof(node) / sizeof(node[0]);
I2cNetworkMaster master(&Wire, node, sizeof(node) / sizeof(node[0]));

FrameIO fio;

uint32_t now = 0, n = 0, then = 0, update = 1000;

long avgWaitTime = 0;

void setup() {
	Serial.begin(115200);
	Wire.begin();
	pinMode(13, OUTPUT);
	Serial.println(freeRam());
	//establishContact();
	delay(2000);
	then = millis();

	//avgWaitTime = averageWaitTime();
}

int index = 0;

void loop() {
	//ratioDropped();
	now = millis();
	index = 0;
	for (uint8_t nodeIndex = 0; nodeIndex < numNodes; nodeIndex++) {
		for (uint8_t packetIndex = 0; packetIndex < node[nodeIndex].numPackets; packetIndex++) {

			n += master.obtainPacket(nodeIndex, packetIndex); // retrieves packet information from slave nodes
			transmitPacket(nodeIndex, packetIndex); // sent packet over Serial
			
			//printPackets(index++);
			//Serial.println(n);
			//Packet* pPacket = &node[nodeIndex].packet[packetIndex];
			//fio.writeDataFrame(nodeIndex, packetindex, pPacket->pStruct, pPacket->size);
		}
	}

	//delay(500);

	//if (now - then > update) {
	//	digitalWrite(13, !digitalRead(13));
	//	then = now;
	//	Serial.println(n);
	//	//delay(500);
	//}
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

void transmitPacket(uint8_t nodeIndex, uint8_t packetIndex) {
	uint8_t  packetSize = node[nodeIndex].packet[packetIndex].size;
	uint8_t* pReadByte = node[nodeIndex].packet[packetIndex].pStruct;
	//Serial.println(packetSize);
	Serial.write(0xA2);
	Serial.write(nodeIndex);
	Serial.write(packetIndex);
	Serial.write(packetSize);
	Serial.write(pReadByte, packetSize);
        Serial.write(0x69);
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
			Serial.print("S0P0D0: ");
			Serial.println(Packet_S0::packet0.data0);
			break;
		case 1:
			Serial.print("S0P1D1: ");
			Serial.println(Packet_S0::packet1.data1);
			Serial.print("S0P1D1: ");
			Serial.println(Packet_S0::packet1.data2);
			break;
		case 2:
			Serial.print("S1P0D0: ");
			Serial.println(Packet_S1::packet0.data0);
			break;
		case 3:
			Serial.print("S1P1D1: ");
			Serial.println(Packet_S1::packet1.data1);
			Serial.print("S1P1D2: ");
			Serial.println(Packet_S1::packet1.data2);
			Serial.print("S1P1D3: ");
			Serial.println(Packet_S1::packet1.data3);
			break;
		case 4:
			Serial.print("S2P0D0[0]: ");
			Serial.println(Packet_S2::packet0.data0[0]);
			Serial.print("S2P0D0[1]: ");
			Serial.println(Packet_S2::packet0.data0[1]);
			Serial.print("S2P0D0[2]: ");
			Serial.println(Packet_S2::packet0.data0[2]);
			Serial.print("S2P0D0[3]: ");
			Serial.println(Packet_S2::packet0.data0[3]);
			Serial.print("S2P0D0[4]: ");
			Serial.println(Packet_S2::packet0.data0[4]);
			Serial.print("S2P0D0[5]: ");
			Serial.println(Packet_S2::packet0.data0[5]);
			break;
		case 5:
			Serial.print("S2P1D1: ");
			Serial.println(Packet_S2::packet1.data1);
			Serial.print("S2P1D2: ");
			Serial.println(Packet_S2::packet1.data2);
			break;
		case 6:
			Serial.print("S2P2D3[0]: ");
			Serial.println(Packet_S2::packet2.data3[0]);
			Serial.print("S2P2D3[1]: ");
			Serial.println(Packet_S2::packet2.data3[1]);
			Serial.print("S2P2D3[2]: ");
			Serial.println(Packet_S2::packet2.data3[2]);
			break;
	}
}

int freeRam() {
	extern int __heap_start, *__brkval;
	int v;
	return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
