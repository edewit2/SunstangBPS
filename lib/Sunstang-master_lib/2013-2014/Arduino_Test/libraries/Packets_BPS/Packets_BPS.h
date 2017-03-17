#ifndef PACKETS_BPS_H
#define PACKETS_BPS_H

#include "Packet.h"
#include "inttypes.h"
#include "SlaveNode.h"

namespace Packet_BPS {

    namespace SensorIndex {
        enum P0 { S0_MAIN  = 0, S1  = 1, S2  = 2, S3  = 3 }; // packet 0 array indexing
        enum P1 { S4       = 0, S5  = 1, S6  = 2, S7  = 3 }; // packet 1 array indexing
        enum P2 { S8       = 0, S9  = 1, S10 = 2, S11 = 3 }; // packet 2 array indexing
        enum P3 { S12      = 0, S13 = 1, S14 = 2, S15 = 3 }; // packet 3 array indexing
    };

    struct Packet_S0Main_TO_S3 {
        int16_t voltage_cell0[4];
        int16_t voltage_cell1[4];
        int16_t temperature[4];
        int16_t s0_current_inst;
        int16_t s0_current_avg;
    } packet_s0_s3;

    struct Packet_S4_TO_S7 { // is this supposed to be Packet_S4_TO_S7?
        int16_t voltage_cell0[4];
        int16_t voltage_cell1[4];
        int16_t temperature[4];
    } packet_s4_s7;

    struct Packet_S8_TO_S11 {
        int16_t voltage_cell0[4];
        int16_t voltage_cell1[4];
        int16_t temperature[4];
    } packet_s8_s11;

    struct Packet_S12_TO_S15 {
        int16_t voltage_cell0[4];
        int16_t voltage_cell1[4];
        int16_t temperature[4];
    } packet_s12_s15;

    Packet packetList[] = {
        Packet(0, (uint8_t*) &packet_s0_s3,   sizeof(packet_s0_s3)), //PACKET_INFO(packet0)),
        Packet(1, (uint8_t*) &packet_s4_s7,   sizeof(packet_s4_s7)), //PACKET_INFO(packet1))
        Packet(2, (uint8_t*) &packet_s8_s11,  sizeof(packet_s8_s11)),
        Packet(3, (uint8_t*) &packet_s12_s15, sizeof(packet_s12_s15))
    };

    SlaveNode slaveNode(0xFD, SETUP(packetList));
    
}
#endif
