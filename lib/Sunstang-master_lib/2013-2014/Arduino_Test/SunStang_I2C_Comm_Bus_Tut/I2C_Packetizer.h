#ifndef I2C_PACKETIZER_H
#define I2C_PACKETIZER_H

//#define details(name) (byte*)&name,sizeof(name)

#include "Arduino.h"
#include <avr/io.h>
#include <Wire.h>



class I2C_Packetizer {
public:
    void begin(uint8_t *, uint8_t, TwoWire *theSerial);
    void sendData(uint8_t address);
    bool receiveData();
private:

    static const uint8_t HEADER = 0x60;
    static const uint8_t HEADER_VERIFY = 0x85;

    TwoWire* wire;

    uint8_t  currentByte;

    uint8_t* packetAddress;  //address of struct
    uint8_t  packetSize;       //size of struct
    uint8_t* rxBuffer; //address for temporary storage and parsing buffer
    uint8_t  rxIndex;  //index for RX parsing buffer
    uint8_t  rxSize;		//RX packet length according to the packet
    uint8_t  checkSum;	   //calculated Checksum
};

#endif