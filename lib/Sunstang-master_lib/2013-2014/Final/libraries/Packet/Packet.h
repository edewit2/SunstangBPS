#ifndef PACKET_H
#define PACKET_H

#include "Arduino.h"
#include "inttypes.h"

#define PACKET_INFO(name) (uint8_t*)&name,sizeof(name)

class Packet {
public:
	uint8_t  id;
	uint8_t* pStruct;
	uint8_t  size;

	Packet(uint8_t _id, uint8_t* _pStruct, uint8_t _size) :
		id(_id), pStruct(_pStruct), size(_size){}
};

#endif
