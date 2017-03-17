#include <EEPROM.h>
#include <Wire.h>
#include <SlaveNode.h>
#include <Packet.h>
#include <I2cNetworkMaster.h>

#include "Packets_Test.h"

#define HIGH_PRIORITY 0

//#define MDEBUG
#define SERIAL_ON

SlaveNode node[] = { Packet_Test::slaveNode };
I2cNetworkMaster master(&Wire, node, sizeof(node)/sizeof(node[0]));

uint8_t requestTable[] = {
	0
};

uint32_t now = 0, then = 0, then2 = 0, update = 10000;

void setup() {
#if defined(MDEBUG) || defined (SERIAL_ON)
	Serial.begin(115200);
	Serial.println("Init!");
#endif
	Wire.begin();
#if defined(MDEBUG) || defined (SERIAL_ON)
	Serial.println("Device Ready!");
#endif
	then = millis();
	then2 = millis();
}

uint32_t n = 0;

void loop() {
	now = millis();
	n += master.obtainPacket(node[0].address,
								 node[0].packet[n&1].id);
#ifdef MDEBUG
		Serial.print("n: ");
		Serial.println(n);
#endif
	if (now - then > update) {
		then = now;
		Serial.print("n: ");
		Serial.println(n);
		n = 0;
		printPacket0();
		printPacket1();
	}
}

void printPacket1() {
	Serial.println(Packet_Test::packet1.longVal);
	for (int i = 0; i < sizeof(Packet_Test::packet1.data)/sizeof(Packet_Test::packet1.data[0]); i++) {
		Serial.println(Packet_Test::packet1.data[i]);
	}
	Serial.println();
}

void printPacket0() {
	Serial.print("data1: ");
	Serial.println(Packet_Test::packet0.data1);
	Serial.print("data2: ");
	Serial.println(Packet_Test::packet0.data2);
	Serial.println();
}