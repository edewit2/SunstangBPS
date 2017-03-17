#include "I2cNetworkMaster.h"

//#define MDEBUG

I2cNetworkMaster::I2cNetworkMaster(TwoWire* _wire, SlaveNode* _slaveNode, uint8_t _numSlaveNodes) {
	wire = _wire;
	node = _slaveNode;
	numSlaveNodes = _numSlaveNodes;
}

bool I2cNetworkMaster::obtainPacket(uint8_t nodeIndex, uint8_t packetIndex) {
#ifdef MDEBUG
	Serial.print("nodeIndex: ");
	Serial.print(nodeIndex);
	Serial.print(", deviceAddress: ");
	Serial.println(node[nodeIndex].address, HEX);
	Serial.print("packetIndex: ");
	Serial.println(packetIndex);

	wire->beginTransmission(node[nodeIndex].address);
	Serial.print("bgnTrns sent: ");
	Serial.println(wire->write(packetIndex));
	Serial.print("endTrns: ");
	Serial.println(wire->endTransmission());
	/*
	0:success
	1:data too long to fit in transmit buffer
	2:received NACK on transmit of address
	3:received NACK on transmit of data
	4:other error
	*/
#else
	wire->beginTransmission(node[nodeIndex].address);
	wire->write(packetIndex);
	uint8_t error = wire->endTransmission();
	if (error) {
		Serial.print("Error: ");
		Serial.print(error);
		Serial.write('\n');
	}
	//delayMicroseconds(500);
#endif

	

	uint8_t  packetSize = node[nodeIndex].packet[packetIndex].size;
	uint8_t* pWriteByte = node[nodeIndex].packet[packetIndex].pStruct;

#ifdef MDEBUG
	Serial.print("packetSize: ");
	Serial.println(packetSize);
	Serial.print("pWriteByte: ");
	Serial.println((uint16_t)pWriteByte, HEX);
	Serial.print("Received Bytes: ");
	Serial.println(
#endif
		wire->requestFrom(node[nodeIndex].address, packetSize)
#ifdef MDEBUG
		);
#else
		;
#endif
	//static const uint16_t timeout = 1000;
	//uint16_t time = millis();
	//while (wire->available() < 1) {
	//	if (millis() - time > timeout) {
	//		return false;
	//	}
	//}

	uint8_t bytesRead = wire->readBytes((char*)pWriteByte, packetSize);
#ifdef MDEBUG
	Serial.print("bytesRead: ");
	Serial.print(bytesRead);
	Serial.write('\n');
#endif

//	if (bytesWritten < packetSize) {
//		return false;
//	}

	/*for (int i = 0; i < packetSize; i++) {
		*(pWriteByte + i) = (uint8_t) wire->read();
	}*/

	//while (wire->available() > 0) { wire->read(); }
	return true;
}
