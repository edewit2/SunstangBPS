#include "I2cNetworkMaster.h"

//#define MDEBUG

I2cNetworkMaster::I2cNetworkMaster(TwoWire* _wire, SlaveNode* _slaveNode, uint8_t _numSlaveNodes) {
	wire = _wire;
	node = _slaveNode;
	numSlaveNodes = _numSlaveNodes;
}

bool I2cNetworkMaster::obtainPacket(uint8_t deviceAddress, uint8_t packetId) {
#ifdef MDEBUG
	Serial.print("deviceAddress: ");
	Serial.println(deviceAddress);
	Serial.print("packetId: ");
	Serial.println(packetId);

	wire->beginTransmission(deviceAddress);
	Serial.print("bgnTrns sent: ");
	Serial.println(wire->write(packetId));
	Serial.print("endTrns: ");
	Serial.println(wire->endTransmission());
#else
	wire->beginTransmission(deviceAddress);
	wire->write(packetId);
	wire->endTransmission();
#endif

	uint8_t  packetSize = node[deviceAddress].packet[packetId].size;
	uint8_t* pWriteByte = node[deviceAddress].packet[packetId].pStruct;

#ifdef MDEBUG
	Serial.print("packetSize: ");
	Serial.println(packetSize);
	Serial.print("pWriteByte: ");
	Serial.println((uint16_t)pWriteByte);
	Serial.print("Received Bytes: ");
	Serial.println(
#endif
		wire->requestFrom(deviceAddress, packetSize)
#ifdef MDEBUG
		);
#else
		;
#endif
	static const uint16_t timeout = 1000;
	uint16_t time = millis();
	while (wire->available() < 1) {
		if (millis() - time > timeout) {
			return false;
		}
	}

	uint8_t bytesWritten = wire->readBytes((char*)pWriteByte, packetSize);

	if (bytesWritten < packetSize) {
		return false;
	}

	/*for (int i = 0; i < packetSize; i++) {
		*(pWriteByte + i) = (uint8_t) wire->read();
	}*/

	while (wire->available() > 0) { wire->read(); }
	return true;
}
