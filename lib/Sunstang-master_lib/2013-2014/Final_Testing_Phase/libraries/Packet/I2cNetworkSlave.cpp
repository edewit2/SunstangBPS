#include "I2cNetworkSlave.h"

#define SDEBUG_INIT
//#define SDEBUG_SEND
//#define SDEBUG_RECV

void I2cNetworkSlave::begin(TwoWire *theSerial, SlaveNode* _slaveNode) {
    wire = theSerial;
	slaveNode = _slaveNode;
	packetId = -1;
	pReadByte = NULL;
#if defined(SDEBUG_SEND) || defined(SDEBUG_RECV) || defined(SDEBUG_INIT)
	Serial.println("_______________");
	Serial.println("Comm Slave Node");
	Serial.print("Addr: "); Serial.println(slaveNode->address,HEX);
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
		Serial.print((uint16_t) pReadByte, HEX);
		Serial.write('\n');
		Serial.print("packetSize: ");
		Serial.print(packetSize);
		Serial.write('\n');
		Serial.print("Bytes Written: ");
		Serial.print(
#endif
		wire->write(pReadByte, packetSize)
#ifdef SDEBUG_SEND
		);
		Serial.write('\n');
#else
		;
#endif
	} else {
		Serial.print("packetId = -1");
		Serial.write('\n');
	}
}

void I2cNetworkSlave::receivedRequestedPacketId(int numBytes) {
	if (numBytes > 1) {
		while (wire->available()) wire->read();
		return;
	}

	packetId = (uint8_t)wire->read();
#ifdef SDEBUG_RECV
	Serial.print("pId: ");
	Serial.print(packetId);
	Serial.write('\n');
	int n = 0;
#endif
//	while (wire->available()) {
//		wire->read();
//#ifdef SDEBUG_RECV
//		n++;
//#endif
//	}
//#ifdef SDEBUG_RECV
//	if (n > 0) {
//		Serial.print("Extra Packets: n=");
//		Serial.println(n);
//	}
//	if (packetId >= slaveNode->numPackets || packetId < 0) {
//		Serial.println("packetId OUT OF BOUNDS");
//		Serial.print("packetRange: 0 - ");
//		Serial.println(slaveNode->numPackets);
//		Serial.print("Received Id: ");
//		Serial.println(packetId);
//	}
//	Serial.println("pId end");
//#endif
}
