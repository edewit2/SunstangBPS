#ifndef PACKETS_S2_H
#define PACKETS_S2_H

#include "Packet.h"
#include "inttypes.h"

namespace Packet_S2 {

	struct Packet_HighPriority {
		uint16_t data0;
	} packet0;

	struct Packet_MedPriority {
		uint16_t data1;
		uint16_t data2;
		uint16_t data3;
	} packet1;

	Packet packetList[] = {
		Packet(0, (uint8_t*) &packet0, sizeof(packet0)),//PACKET_INFO(packet0)),
		Packet(1, (uint8_t*) &packet1, sizeof(packet1))//PACKET_INFO(packet1))
	};

	SlaveNode slaveNode(0xFE, SETUP(packetList));
	
}
#endif
