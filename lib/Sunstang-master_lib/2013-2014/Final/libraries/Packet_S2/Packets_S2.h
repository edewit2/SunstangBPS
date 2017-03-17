#ifndef PACKETS_S2_H
#define PACKETS_S2_H

#include "Packet.h"
#include "inttypes.h"

namespace Packet_S2 {

	struct Packet_HighPriority {
		uint8_t data0[6];
	} packet0;

	struct Packet_MedPriority {
		float   data1;
		uint8_t data2;
	} packet1;

	struct Packet_LowPriority {
		float   data3[3];
	} packet2;

	Packet packetList[] = {
		Packet(0, (uint8_t*) &packet0, sizeof(packet0)),
		Packet(1, (uint8_t*) &packet1, sizeof(packet1)),
		Packet(2, (uint8_t*) &packet2, sizeof(packet2))
	};

	SlaveNode slaveNode(0x85, SETUP(packetList));
	
}
#endif
