
#define SLAVE

#include <EEPROM.h>
#include <Wire.h>
#include "SlaveNode.h"
#include "I2cNetworkSlave.h"
#include "TimedFunction.h"

#include "Packets_Bps_Gordon.h"

using namespace Packets_Bps_Gordon;

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
	packet0.batteryVoltageRaw = n;//analogRead(0);
	packet0.batteryTempRaw = 255-n;//analogRead(1);
	n++;
}

void updatePacket1() {
	static int n = 0;
	Packets_Bps_Gordon::packet1.longVal += n;//3.14159265358979323846264338327950;
	for (int i = 0; i < sizeof(Packets_Bps_Gordon::packet1.data) / sizeof(Packets_Bps_Gordon::packet1.data[0]); i++) {
		Packets_Bps_Gordon::packet1.data[i] = n + i;
	}
	n++;
}

void readPacketId(int numBytes) {
	comm.receivedRequestedPacketId();
}

void sendPacket() {
	comm.sendRequestedPacket();
}
