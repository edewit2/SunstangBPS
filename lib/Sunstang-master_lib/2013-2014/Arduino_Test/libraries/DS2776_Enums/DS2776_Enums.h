#ifndef DS2776_ENUMS_H
#define DS2776_ENUMS_H

#include "Arduino.h"

enum RelayState { OPEN = HIGH, CLOSED = LOW };

//namespace ReadResponse {
//    enum EReadResponse {
//        NO_DATA = 0,
//        GENERATE_RESET_PULSE_FAILURE,
//        SET_DATA_MODE_FAILURE,
//        INCOMPLETE_DATA,
//        READ_SUCCESS
//    };
//}
//
//namespace HostRx {
//    enum EHostRx {
//        GENERATE_RESET_PULSE_0   = 0xCD,
//        GENERATE_RESET_PULSE_1   = 0xED,
//        GENERATE_PROGRAM_PULSE   = 0xFD,
//        SKIP_ROM_COMMAND         = 0xCC,
//        SET_COMMAND_MODE         = 0xE3
//    };
//}
//
//namespace DataUpdateStatus {
//    enum EDataUpdateStatus { SUCCESS = true, FAILURE = false };
//}

#endif