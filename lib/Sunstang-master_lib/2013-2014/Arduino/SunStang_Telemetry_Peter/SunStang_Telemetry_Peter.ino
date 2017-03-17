
#define SLAVE

#include <EEPROM.h>
#include <Wire.h>
#include "SlaveNode.h"
#include "I2cNetworkSlave.h"
#include "TimedFunction.h"

#include "Packets_Telemetry_Peter.h"

using namespace Packet_Telemetry_Peter;

I2cNetworkSlave comm;

TimedFunction fn[] = {
	TimedFunction(updatePacket0, 1000),
	TimedFunction(updatePacket1, 500)
};
uint8_t numFuncs = sizeof(fn) / sizeof(fn[0]);

uint32_t systemTime = 0;

void setup() {
	Serial.begin(115200);
	Serial.println("Init");

	comm.begin(&Wire, &slaveNode);

	Wire.begin(slaveNode.address);
	Wire.onReceive(readPacketId);
	Wire.onRequest(sendPacket);

	for (int i = 0; i < numFuncs; i++) {
		fn[i].start();
        }
        
	Serial.println("Ready!");
}

void loop() {
	systemTime = millis();
	for (uint8_t i = 0; i < numFuncs; i++) {
		fn[i].update(systemTime);
	}
}

void updatePacket0() {
	static int n = 0;
	packet0.voltageInst = n;//analogRead(0);
	packet0.time = 255-n;//analogRead(1);
	n++;
}

void updatePacket1() {
	static int n = 0;
	//packet1.longVal += n;//3.14159265358979323846264338327950;
	for (int i = 0; i < sizeof(packet1.voltageAvg) / sizeof(packet1.voltageAvg[0]); i++) {
		packet1.voltageAvg[i] = n + i;
	}
	n++;
}

void readPacketId(int numBytes) {
	comm.receivedRequestedPacketId();
}

void sendPacket() {
	comm.sendRequestedPacket();
}
