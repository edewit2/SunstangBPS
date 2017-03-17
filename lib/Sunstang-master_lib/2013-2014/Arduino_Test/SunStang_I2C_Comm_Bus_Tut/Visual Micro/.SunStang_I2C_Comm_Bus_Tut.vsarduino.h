#ifndef _VSARDUINO_H_
#define _VSARDUINO_H_
//Board = Arduino Uno
#define __AVR_ATmega328p__
#define __AVR_ATmega328P__
#define ARDUINO 103
#define ARDUINO_MAIN
#define __AVR__
#define __avr__
#define F_CPU 16000000L
#define __cplusplus
#define __inline__
#define __asm__(x)
#define __extension__
#define __ATTR_PURE__
#define __ATTR_CONST__
#define __inline__
#define __asm__ 
#define __volatile__

#define __builtin_va_list
#define __builtin_va_start
#define __builtin_va_end
#define __DOXYGEN__
#define __attribute__(x)
#define NOINLINE __attribute__((noinline))
#define prog_void
#define PGM_VOID_P int
            
typedef unsigned char byte;
extern "C" void __cxa_pure_virtual() {;}

//
//
void updatePacket0();
void updatePacket1();
void readPacketId(int numBytes);
void sendPacket();

#include "D:\Program Files\arduino-1.0.3\hardware\arduino\cores\arduino\arduino.h"
#include "D:\Program Files\arduino-1.0.3\hardware\arduino\variants\standard\pins_arduino.h" 
#include "D:\My Documents\GitHub\SunStang\Arduino_Test\SunStang_I2C_Comm_Bus_Tut\SunStang_I2C_Comm_Bus_Tut.ino"
#include "D:\My Documents\GitHub\SunStang\Arduino_Test\SunStang_I2C_Comm_Bus_Tut\I2C_Packetizer.cpp"
#include "D:\My Documents\GitHub\SunStang\Arduino_Test\SunStang_I2C_Comm_Bus_Tut\I2C_Packetizer.h"
#include "D:\My Documents\GitHub\SunStang\Arduino_Test\SunStang_I2C_Comm_Bus_Tut\Packets_Template.h"
#endif
