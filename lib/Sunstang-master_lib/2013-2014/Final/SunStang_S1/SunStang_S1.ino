#define SLAVE

#include <EEPROM.h>
#include <Wire.h>
#include "SlaveNode.h"
#include "I2cNetworkSlave.h"
#include "TimedFunction.h"

#include "Packets_S1.h"

using namespace Packet_S1;

I2cNetworkSlave comm;

TimedFunction fn[] = {
	TimedFunction(updatePacket0, 1000),
	TimedFunction(updatePacket1, 500),
	TimedFunction(heartBeat, 500)
};
uint8_t numFuncs = sizeof(fn) / sizeof(fn[0]);

uint32_t systemTime = 0;

void setup() {
	comm.begin(&Wire, &slaveNode);

	Wire.begin(slaveNode.address);
	Wire.onReceive(readPacketId);
	Wire.onRequest(sendPacket);

	pinMode(13, OUTPUT);

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
	static uint16_t n = 0x0100;
	packet0.data0 = n;
	n++;
}

void updatePacket1() {
	static uint16_t n = 0xAAAA;
	packet1.data1 = n;
	packet1.data2 = n + 2;
	packet1.data3 = n + 4;
	n++;
}

void heartBeat() {
	digitalWrite(13, !digitalRead(13));
}

void readPacketId(int numBytes) {
	comm.receivedRequestedPacketId(numBytes);
}

void sendPacket() {
	comm.sendRequestedPacket();
}

