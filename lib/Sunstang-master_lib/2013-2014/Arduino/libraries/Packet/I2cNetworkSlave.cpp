#include "I2cNetworkSlave.h"

//#define SDEBUG_SEND
//#define SDEBUG_RECV

void I2cNetworkSlave::begin(TwoWire *theSerial, SlaveNode* _slaveNode) {
    wire = theSerial;
	slaveNode = _slaveNode;
	packetId = -1;
	pReadByte = NULL;
#if defined(SDEBUG_SEND) || defined(SDEBUG_RECV)
	Serial.println("_______________");
	Serial.println("Comm Slave Node");
	Serial.print("Addr: "); Serial.println(slaveNode->address);
	Serial.print("numPackets: "); Serial.println(slaveNode->numPackets);
	Serial.println("---------------");
#endif
}

void I2cNetworkSlave::sendRequestedPacket() {
	if (packetId != -1) {
		pReadByte  = slaveNode->packet[packetId].pStruct;
		packetSize = slaveNode->packet[packetId].size;
#ifdef SDEBUG_SEND
		Serial.print("pReadByte: ");
		Serial.println((uint16_t) pReadByte);
		Serial.print("packetSize: ");
		Serial.println(packetSize);
		Serial.print("Bytes Written: ");
		Serial.println(
#endif
		wire->write(pReadByte, packetSize)
#ifdef SDEBUG_SEND
		);
#else
		;
#endif
	} else {
		Serial.println("packetId = -1");
	}
}

void I2cNetworkSlave::receivedRequestedPacketId() {
	packetId = (uint8_t)wire->read();
#ifdef SDEBUG_RECV
	Serial.print("pId: ");
	Serial.println(packetId);
	int n = 0;
#endif
	while (wire->available()) {
		wire->read();
#ifdef SDEBUG_RECV
		n++;
#endif
	}
#ifdef SDEBUG_RECV
	if (n > 0) {
		Serial.print("Extra Packets: n=");
		Serial.println(n);
	}
	if (packetId >= slaveNode->numPackets || packetId < 0) {
		Serial.println("packetId OUT OF BOUNDS");
		Serial.print("packetRange: 0 - ");
		Serial.println(slaveNode->numPackets);
		Serial.print("Received Id: ");
		Serial.println(packetId);
	}
#endif
}
