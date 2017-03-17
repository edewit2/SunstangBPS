#ifndef DS2776_H
#define DS2776_H

#include "Packets_BPS.h"
#include "DS2776_Enums.h"

const float   NOMINAL_VOLTAGE       = 3.7;        // V
const float   NOMINAL_TEMPERATURE   = 20;         // Degrees Celcius
const float   VOLTAGE_PER_INT16     = 4.883 / 1000; // V/int
const float   TEMPERATURE_PER_INT16 = 0.125;      // Degrees Celcius/int
const float   R_SNS                 = 1.5 / 1000; // Sense Resistance (ohms)

extern HardwareSerial Serial;

class DS2776 {
private:
    enum ReadResponse {
        NO_DATA = 0,
        GENERATE_RESET_PULSE_FAILURE,
        SET_DATA_MODE_FAILURE,
        INCOMPLETE_DATA,
        READ_SUCCESS
    };
    enum HostRx {
        GENERATE_RESET_PULSE_0   = 0xCD,
        GENERATE_RESET_PULSE_1   = 0xED,
        GENERATE_PROGRAM_PULSE   = 0xFD,
        SKIP_ROM_COMMAND         = 0xCC,
        SET_COMMAND_MODE         = 0xE3
    };

    // The addresses for the different measurements
    static const uint8_t CURRENT_AVG_MSB   = 0x08;
    static const uint8_t CURRENT_AVG_LSB   = 0x09;
    static const uint8_t TEMP_MSB          = 0x0A;
    static const uint8_t TEMP_LSB          = 0x0B;
    static const uint8_t VCELL_1_MSB       = 0x0C; // Vin1 - Vss
    static const uint8_t VCELL_1_LSB       = 0x0D;
    static const uint8_t CURRENT_INST_MSB  = 0x0E;
    static const uint8_t CURRENT_INST_LSB  = 0x0F;
    static const uint8_t VCELL_2_MSB       = 0x1C; // Vin2 - Vin1
    static const uint8_t VCELL_2_LSB       = 0x1D;

    // parameters for requesting all data
    static const uint8_t START_ADDRESS     = CURRENT_AVG_MSB;
    static const uint8_t NUM_REQUEST_BYTES = VCELL_2_LSB - CURRENT_AVG_MSB + 1;

    // unique id of the chip. To be determined beforehand and hardcoded.
    union ID {
        uint8_t  t8[8];
        uint64_t t64;
        ID(const uint64_t _id) : t64(_id) {}
        ID(const uint8_t _id8[8]) : t64((uint64_t)_id8) {}
    };
    const ID id;

private: // START OF PRIVATE METHODS
    // Sends a request to the bridge asking for all data
    void _requestNewValues() {

        static const byte sendBytes[] = {
            0xC1, //Tell Bridge to send reset pulse to 1-wire bus i.e. start of tx 
            0xE1, //Tell Bridge enter data mode 
            0x55, //To the chips match rom command
            0x3D,
            0x5B,
            0xF3,
            0x00,
            0x60,
            0x07,
            0x00,
            0xD9,
            0x69, //read memory address command - to the chip
            0x08,  //ADDRESS MSB, for some reason with this command if I give the chip the MSB, it auto increments to get the LSB
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xFF, //buffer for 1 byte data return
            0xE3, //Tell bridge to go back into commmand mode
            0xC1 //Tell Bridge to send reset pulse to 1-wire bus i.e. end of tx
        };

        for (int i = 0; i < sizeof(sendBytes)/sizeof(sendBytes[0]); i++) {
            Serial.write(sendBytes[i]);
            delay(20);
        }

        /*
        digitalWrite(12, HIGH);

        //static byte c = 0;

        Serial.write(0xC1);          // Reset DS2480 commands (start of TX)
        Serial.write(0xE1);          // Enter Data Mode
        Serial.write(0x55);          // Address to follow is chip we want to talk to
        
        //delay(1000);
        for (int8_t i = 7; i >= 0; i--) {
            Serial.write(id.t8[i]);
        }
        //delay(1000);

        //Serial.write(&id.t8[7], 8);      // Write 8-byte address to DS2480
        Serial.write(0x69);          // Read following memory address from chip
        Serial.write(START_ADDRESS); // ROM to access

        //Serial.write(c++);
        for (uint8_t i = 0; i < NUM_REQUEST_BYTES; i++) {
            Serial.write(0xFF);      // Create return space
        }

        Serial.write(0xE3);          // DS2480 back to command mode 
        Serial.write(0xC1);          // DS2480 send reset pulse on 1-wire bus. (End of TX)
        */
    }

    // Reads a response containing from the bridge
    ReadResponse _readResponse() {
        byte inByte = 0;

        uint32_t time = millis();

        while (!(Serial.available() > 34) && millis() - time < 100) {}

        if (!Serial.available()) {
            return DS2776::NO_DATA;
        }
        /*if (Serial.available() != 35) {
            return DS2776::INCOMPLETE_DATA;
        }*/

        inByte = Serial.read();
        if (!(inByte == DS2776::GENERATE_RESET_PULSE_0 ||
            inByte == DS2776::GENERATE_RESET_PULSE_1)) {
            return DS2776::GENERATE_RESET_PULSE_FAILURE;
        }
        for (int i = 0; i < 8; i++) {
            if (Serial.read() != id.t8[i])
                return DS2776::INCOMPLETE_DATA;
        }

        byte msb = Serial.read();
        byte lsb = Serial.read();
        current_average = decodeCurrent(msb, lsb);

        msb = Serial.read();
        lsb = Serial.read();
        temperature = decodeVoltageOrTemperature(msb, lsb);

        msb = Serial.read();
        lsb = Serial.read();
        voltage_cell_0 = decodeVoltageOrTemperature(msb, lsb);

        msb = Serial.read();
        lsb = Serial.read();
        current_instantaneous = decodeCurrent(msb, lsb);

        for (int i = 0; i < VCELL_2_MSB - CURRENT_AVG_LSB; i++) {
            Serial.read(); // throw away data
        }

        msb = Serial.read();
        lsb = Serial.read();
        voltage_cell_1 = decodeVoltageOrTemperature(msb, lsb);

        inByte = Serial.read();
        if (!(inByte == DS2776::GENERATE_RESET_PULSE_0 ||
            inByte == DS2776::GENERATE_RESET_PULSE_1)) {
            return DS2776::GENERATE_RESET_PULSE_FAILURE;
        }

        flushRxBuffer(); // supplementary flush buffer to get rid of trailing data pieces

        return DS2776::READ_SUCCESS;
    }

    void write_setup1(byte address, byte data) {
        // this function DOES work
        byte sendBytes[] ={
            0x29, // Set Vpp for 512us
            0xC1, // Tell Bridge to send reset pulse to 1-wire bus i.e. start of tx 
            0xE1, // Tell Bridge enter data mode 
            0xCC, // To the chips skip rom command / when we have all the devices on a bus, we will have to use a different command to address the specific device we want to hear from
            0x6C, // write to memory address

            address, // ADDRESS MSB, for some reason with this command if I give the chip the MSB, it auto increments to get the LSB
            data,    // Data byte

            0xFF, // buffer for 1 byte data return
            0xFF,
            0xE3, // Tell bridge to go back into commmand mode
            0xFD, // Generate Program Pluse
            0xE1, // Set Data mode
            0xFF, // Read sent byte
            0xE3, // Tell bridge to go back into commmand mode
            0xC1, // Reset pulse
            0x00
        };

        for (int i=0; i < sizeof(sendBytes)/sizeof(sendBytes[0]); i++) {
            Serial.write(sendBytes[i]);
            delay(20); //I have tried a lower delay before (10ms) and it was fine
        }
    }

    int16_t decodeVoltageOrTemperature(byte msb, byte lsb) {
        uint16_t temp = word(msb, lsb);
        uint16_t output = (temp >> 5) & 0x03FF;
        output = (temp & 0x8000) ^ output;
        return output;
    }

    int16_t decodeCurrent(byte msb, byte lsb) {
        return word(msb, lsb); // lets make this a defined function or inline or whatever
    }

    void flushRxBuffer() {
        while (Serial.available()) {
            Serial.read(); // flush Rx Buffer
        }
    }

public:
    enum DataUpdateStatus { SUCCESS = true, FAILURE = false };

    // The local copies of the sensor measurements. These variables form the model of the battery
    // IMPORTANT NOTE: these are raw data values
    int16_t voltage_cell_0;
    int16_t voltage_cell_1;
    int16_t temperature;
    int16_t current_instantaneous;
    int16_t current_average;

    // Constructor: Initializes the sensor and model
    DS2776(uint64_t inputId) : id(inputId) {
        // initialize model
        this->voltage_cell_0        = (int)(NOMINAL_VOLTAGE / VOLTAGE_PER_INT16);
        this->voltage_cell_1        = (int)(NOMINAL_VOLTAGE / VOLTAGE_PER_INT16);
        this->temperature           = (int)(NOMINAL_TEMPERATURE / TEMPERATURE_PER_INT16);
        this->current_instantaneous = 0;
        this->current_average       = 0;
    }

    void init() {
        //intialize the DS2480B bridge
        Serial.flush();
        Serial.write(0xC1);
        delay(2);
        Serial.flush();

        //I can't recall what each byte does, but on start of of the serial-1-wire bridge,
        //these commands initalize the bridge
        static const byte data[5] = {
            0x17,
            0x45,
            0x5B,
            0x0F,
            0x91
        };
        Serial.write(data, 5);

        delay(20);

        write_setup1(0x60, 0x0C);  //*Control Register (00001100-->0x0C), undervoltage condition: 2.60V
        write_setup1(0x64, 0xD7);  //*Charge Voltage Register (4.19V)
        write_setup1(0x65, 0x01);  //Current-to-detect fully charged state
        write_setup1(0x66, 0x96);  //Active-Empty Voltage (2.925 V)
        write_setup1(0x67, 0x14);  //Active-Empty Current Register (IAE) guess ~ (0.8A-2.6A) 
        write_setup1(0x69, 0xFF);  //RSNSP (highest I can set is 3.9mOhms ... ours is actually 1.5mOhms)
        write_setup1(0x78, 0x20);  //OVERCURRENT PROTECTION   (value not perfectly set, controller thinks shut off at 12.2A, might actually be 33A at max Vsns)        
        write_setup1(0x7F, 0x5A);  //*Overvoltage Threshold Register (4.189V)
        write_setup1(0x7F, 0x5A);

        //write_setup1(0x60, 0x0C);
        //write_setup1(0x64, 0xD4);
        //write_setup1(0x65, 0x01);
        //write_setup1(0x66, 0x96);
        //write_setup1(0x67, 0xFA);
        //write_setup1(0x69, 0xC8);
        //write_setup1(0x78, 0x10);  //OVERCURRENT PROTECTION   (CURRENTLY SET TO 10A)        
        //write_setup1(0x7F, 0x5A);
        //write_setup1(0x7F, 0x5A);
    }

    DataUpdateStatus updateAllData() {
        byte count = 0;

        flushRxBuffer();

        do {
            _requestNewValues(); // Make a request to the chip
            count++;
        } while (false);
        //_readResponse() != DS2776::READ_SUCCESS && count < 3);  // Read the response from the bridge

        if (count >= 3) {
            //Serial.println("\nDS2776::FAILURE");
            return DS2776::FAILURE;
        }

        return DS2776::SUCCESS;
    }

    // Returns true if the battery pack is charging
    bool isCharging() {
        return this->current_instantaneous < 0;
    }
};

#endif
