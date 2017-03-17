#ifndef SLAVE_NODE_H
#define SLAVE_NODE_H

#include "Packet.h"
#include <EEPROM.h>

#define SETUP(list) list,sizeof(list)/sizeof(list[0])

class SlaveNode {
public:
	uint8_t address;
	Packet* packet;
	uint8_t numPackets;
	SlaveNode(uint8_t _address, Packet _packet[], uint8_t _numPackets) {
#ifdef SLAVE
		if (EEPROM.read(0x00) != _address) {
			EEPROM.write(0x00, _address);
		}
#endif
		address = _address;
		packet = _packet;
		numPackets = _numPackets;
	}
};



#endif