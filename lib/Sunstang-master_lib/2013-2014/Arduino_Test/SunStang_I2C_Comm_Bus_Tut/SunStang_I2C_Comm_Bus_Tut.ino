
#define SLAVE
//#define SDEBUG_SEND
//#define SDEBUG_RECV

#include <EEPROM.h>
#include <Wire.h>
#include "SlaveNode.h"
#include "I2cNetworkSlave.h"
#include "TimedFunction.h"

#include "Packets_Test.h"

using namespace Packet_Test;

I2cNetworkSlave comm;

TimedFunction fn[] = {
	TimedFunction(updatePacket0, 1000),
	TimedFunction(updatePacket1, 500)
};
uint8_t numFuncs = sizeof(fn) / sizeof(fn[0]);

uint32_t systemTime = 0;

void setup() {
#if defined(SDEBUG_RECV) || defined(SDEBUG_SEND)
	Serial.begin(115200);
	Serial.println("Init");

	Serial.print("p0  adr: "); Serial.println((uint16_t) &packet0);
	Serial.print("p[] adr: "); Serial.println((uint16_t) packetList[0].pStruct);
	Serial.print("snp0adr: "); Serial.println((uint16_t) slaveNode.packet[0].pStruct);

#endif

	comm.begin(&Wire, &slaveNode);

	Wire.begin(slaveNode.address);
	Wire.onReceive(readPacketId);
	Wire.onRequest(sendPacket);

	for (int i = 0; i < numFuncs; i++)
		fn[i].start();
#if defined(SDEBUG_RECV) || defined(SDEBUG_SEND)
	Serial.println("Ready!");
#endif
}

void loop() {
	systemTime = millis();
	for (uint8_t i = 0; i < numFuncs; i++) {
		fn[i].update(systemTime);
	}
}

void updatePacket0() {
	static int n = 0;
	packet0.data1 = n;//analogRead(0);
	packet0.data2 = 255-n;//analogRead(1);
	n++;
	//Serial.println((uint16_t)(packetList[0].pStruct) );
	//Serial.println((uint16_t)&packet0);
}

void updatePacket1() {
	static int n = 0;
	Packet_Test::packet1.longVal += n;//3.14159265358979323846264338327950;
	for (int i = 0; i < sizeof(Packet_Test::packet1.data) / sizeof(Packet_Test::packet1.data[0]); i++) {
		Packet_Test::packet1.data[i] = n + i;
	}
	n++;
}

void readPacketId(int numBytes) {
#ifdef SDEBUG_RECV
	Serial.println("readPacketId");
#endif
	comm.receivedRequestedPacketId();
}

void sendPacket() {
#ifdef SDEBUG_SEND
	Serial.println("sendPacket");
#endif
	comm.sendRequestedPacket();
}

/*
void I2cNetworkSlave::sendRequestedPacket() {
if (packetId != -1) {
pReadByte  = slaveNode->packet[packetId].pStruct;
packetSize = slaveNode->packet[packetId].size;
wire->write(pReadByte, packetSize);
}
}

void I2cNetworkSlave::receivedRequestedPacketId() {

while (wire->available()) packetId = (uint8_t)wire->read();
if (packetId >= slaveNode->numPackets) {
packetId = 0;
}
Serial.println(packetId);
}*/