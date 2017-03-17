
#define SLAVE

#include <EEPROM.h>
#include <Wire.h>
#include "SlaveNode.h"
#include "I2cNetworkSlave.h"
#include "TimedFunction.h"

#include "Packets_S0.h"

using namespace Packet_S0;

I2cNetworkSlave comm;

TimedFunction fn[] = {
	TimedFunction(updatePacket0, 1),
	TimedFunction(updatePacket1, 500),
	TimedFunction(heartBeat, 250)
};
uint8_t numFuncs = sizeof(fn) / sizeof(fn[0]);

uint32_t systemTime = 0;

void setup() {

	Serial.begin(115200);

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
	//noInterrupts();
	//Serial.print("updatePacket0: ");
	static uint8_t n = 0;
	//packet0.data0 = n;
	packet0.data0 = analogRead(0)/4;
	//Serial.println(packet0.data0);
	n++;
	//interrupts();
}

void updatePacket1() {
	//noInterrupts();
	//Serial.print("updatePacket1: ");
	static uint8_t c = 0;
	packet1.data1 = c;
	packet1.data2 = c + 5;
	//Serial.print(packet1.data1);
	//Serial.print(", ");
	//Serial.println(packet1.data2);
	c++;
	//interrupts();
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

