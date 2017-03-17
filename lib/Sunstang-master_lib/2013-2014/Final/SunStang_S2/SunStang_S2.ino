
#define SLAVE

#include <EEPROM.h>
#include <Wire.h>
#include "SlaveNode.h"
#include "I2cNetworkSlave.h"
#include "TimedFunction.h"

#include "Packets_S2.h"

using namespace Packet_S2;

I2cNetworkSlave comm;

TimedFunction fn[] = {
	TimedFunction(updatePacket0, 1000),
	TimedFunction(updatePacket1, 500),
	TimedFunction(updatePacket2, 1000),
	TimedFunction(heartBeat, 1000)
};
uint8_t numFuncs = sizeof(fn) / sizeof(fn[0]);

uint32_t systemTime = 0;

void setup() {
	comm.begin(&Wire, &slaveNode);

	Wire.begin(slaveNode.address);
	Wire.onReceive(readPacketId);
	Wire.onRequest(sendPacket);

	for (int i = 0; i < numFuncs; i++)
		fn[i].start();
}

void loop() {
	systemTime = millis();
	for (uint8_t i = 0; i < numFuncs; i++) {
		fn[i].update(systemTime);
	}
}

void updatePacket0() {
	static uint8_t n = 0;
	for (int i = 0; i < 6; i++) {
		packet0.data0[i] = n+i;
	}
	n++;
}

void updatePacket1() {
	static uint8_t n = 0;
	static float   f = 3.14159265358979323842624338327950;
	packet1.data1 = f++;
	packet1.data2 = n + 5;
	n++;
}

void updatePacket2() {
	static float f = 2.718;
	for (int i = 0; i < 3; i++) {
		packet2.data3[i] = f + i;
	}
	f++;
}

void readPacketId(int numBytes) {
	comm.receivedRequestedPacketId(numBytes);
}

void sendPacket() {
	comm.sendRequestedPacket();
}

void heartBeat() {
	digitalWrite(13, !digitalRead(13));
}
