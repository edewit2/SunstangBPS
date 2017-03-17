#include "I2C_Packetizer.h"

void I2C_Packetizer::begin(uint8_t * ptr, uint8_t length, TwoWire *theSerial) {
    packetAddress = ptr;
    packetSize = length;
    wire = theSerial;

    rxBuffer = (uint8_t*) malloc(packetSize);
}

void I2C_Packetizer::sendData(uint8_t i2cAddress) {
    uint8_t checkSum = packetSize;
    wire->beginTransmission(i2cAddress);
    wire->write(HEADER);
    wire->write(HEADER_VERIFY);
    wire->write(packetSize);
    for (int i = 0; i < packetSize; i++) {
        currentByte = *(packetAddress+i);
        checkSum ^= currentByte;
        wire->write(currentByte);
    }
    wire->write(checkSum);
    wire->endTransmission();
}

bool I2C_Packetizer::receiveData() {
    if (rxSize == 0 && wire->available() >= 3) {
        while (wire->read() != HEADER) {
            if (wire->available() < 3) return false;
        }
        if (wire->read() == HEADER_VERIFY) {
            rxSize = wire->read();
        }
        if (rxSize != packetSize) {
            rxSize = 0;
            return false;
        }
        if (rxSize != 0) {
            while (wire->available() && rxIndex <= rxSize) {
                rxBuffer[rxIndex++] = wire->read();
            }
            if (rxSize == rxIndex-1) {
                checkSum = rxSize;
                for (int i = 0; i < rxSize; i++) {
                    checkSum ^= rxBuffer[i];
                }
                if (checkSum == rxBuffer[rxIndex-1]) {
                    memcpy(packetAddress, rxBuffer, packetSize);
                    rxSize = 0;
                    rxIndex = 0;
                    return true;
                } else {
                    rxSize = 0;
                    rxIndex = 0;
                    return false;
                }
            }
        }
    }
    return false;
}
