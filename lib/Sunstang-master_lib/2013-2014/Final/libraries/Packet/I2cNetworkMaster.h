#ifndef I2C_NETWORK_MASTER_H
#define I2C_NETWORK_MASTER_H

#include "Wire.h"
#include "SlaveNode.h"

class I2cNetworkMaster {
public:
	TwoWire* wire;
	SlaveNode* node;
	uint8_t numSlaveNodes;

	I2cNetworkMaster(TwoWire* twoWire, SlaveNode* _slaveNode, uint8_t numSlaveNodes);
	bool obtainPacket(uint8_t nodeIndex, uint8_t packetIndex);
};

#endif