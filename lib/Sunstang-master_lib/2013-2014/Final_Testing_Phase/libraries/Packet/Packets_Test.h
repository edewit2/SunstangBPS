#ifndef PACKETS_TEST_H
#define PACKETS_TEST_H

#include "Packet.h"
#include "inttypes.h"
#include "SlaveNode.h"

namespace Packet_Test {

	struct Packet_HighPriority {
		uint8_t data1;
		uint8_t data2;
	} packet0;

	struct Packet_MedPriority {
		float longVal;
		int   data[10];
	} packet1;

	Packet packetList[] = {
		Packet(0, (uint8_t*) &packet0, sizeof(packet0)),//PACKET_INFO(packet0)),
		Packet(1, (uint8_t*) &packet1, sizeof(packet1))//PACKET_INFO(packet1))
	};

	SlaveNode slaveNode(0x00, SETUP(packetList));
	
}
#endif
