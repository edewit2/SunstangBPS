#ifndef I2C_NETWORK_SLAVE_H
#define I2C_NETWORK_SLAVE_H

#include "SlaveNode.h"
#include "Wire.h"
#include "inttypes.h"

class I2cNetworkSlave {
public:
    void begin(TwoWire* _wire, SlaveNode* _slaveNode);
    void sendRequestedPacket();
	void receivedRequestedPacketId(int numBytes);

private:
    TwoWire*   wire;
	SlaveNode* slaveNode;

	uint8_t    packetId;
	uint8_t    packetSize;
    uint8_t*   pReadByte;
};

#endif
