#include <TimedFunction.h>
#include <EEPROM.h>
#include <Wire.h>
#include <SlaveNode.h>
#include <Packet.h>
#include <I2cNetworkMaster.h>

#include "Packets_Test.h"

#define CARRIAGE_RETURN 13

SlaveNode node[] = {Packet_Test::slaveNode};
I2cNetworkMaster master(&Wire, node, sizeof(node) / sizeof(node[0]));

uint8_t requestTable[] = {
	0
};
uint32_t now = 0, n = 0, then = 0, update = 100;

void setup() {
	Serial.begin(115200);
	Wire.begin();
	establishContact();
	pinMode(13, OUTPUT);
}

void loop() {
	now = millis();
	n += master.obtainPacket(node[0].address,
							 node[0].packet[n & 1].id);
	if (now - then > update) {
		then = now;
		transmitPacket(0, 0);
	}
}

void transmitPacket(uint8_t nodeIndex, uint8_t packetId) {
	uint8_t  packetSize = node[nodeIndex].packet[packetId].size;
	uint8_t* pReadByte  = node[0].packet[packetId].pStruct;
	//Serial.println(packetSize);
	Serial.write(0xAF);
	Serial.write(0xF4);
	Serial.write(nodeIndex);
	Serial.write(packetId);
	Serial.write(packetSize);
	Serial.write(pReadByte, packetSize);
	Serial.write(CARRIAGE_RETURN);
}

void establishContact() {
	while (Serial.available() <= 0) {
		Serial.print('A');   // send a capital A
		Serial.print('\n');
		delay(300);
		then = millis();
		digitalWrite(13, HIGH);
	}
}
