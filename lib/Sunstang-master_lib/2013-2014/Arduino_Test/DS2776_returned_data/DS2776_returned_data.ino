#include <SoftwareSerial.h>

SoftwareSerial softSerial(2, 3); // rx, tx, respectively

struct Cmd {
    uint8_t command, num;
    Cmd(uint8_t c, uint8_t n=1) : command(c), num(n) {}
};

Cmd commands[] = {
    Cmd(0xC1), // Reset DS2480 commands (start of TX)
    Cmd(0xE1), // Enter Data Mode
    Cmd(0x55), // Address to follow is chip we want to talk to
    Cmd(0x3D), // Write 8-byte address to DS2480
    Cmd(0x07), // This ID is for board #3
    Cmd(0xF0),
    Cmd(0x00),
    Cmd(0x60),
    Cmd(0x07),
    Cmd(0x00),
    Cmd(0x00),
    Cmd(0xC4),
    Cmd(0x69), // Read following memory address from chip
    Cmd(0xC1), // start address of ROM to access
    Cmd(0xFF, 22), // Create return space for 22 bytes data --> ask john why this is important
    Cmd(0xE3), // DS2480 back to command mode
    Cmd(0xC1)  // DS2480 send reset pulse on 1-wire bus. (End of TX)
};

void setup()
{
    Serial.begin(9600);
    softSerial.begin(9600);
    delay(1000);
}

void loop() {
    for (int i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) {
        for (int j = 0; j < commands[i].num; j++) {
            softSerial.write(commands[i].command);
            Serial.print(commands[i].command, HEX);
            delay(500);
            if (softSerial.available()) {
                Serial.print(" ");
                Serial.println(softSerial.read(), HEX);
            } else {
                Serial.println();
            }
        }
    }

    //while (softSerial.available()) {
    //    Serial.println(softSerial.read());
    //}
    Serial.println("End of requests.  Begin of excess echos:");
    while (softSerial.available()) {
        Serial.println(softSerial.read(), HEX);
    }

    Serial.println();
    Serial.println();
    Serial.println();
    delay(3000);
}

void diagnose(uint8_t cmd) {
    delay(50);
    int count = 0;
    do {
        Serial.print(cmd, HEX);
        if (softSerial.available()) {
            Serial.print('\t');
            if (count == 0) {
                Serial.println(softSerial.read());
            } else {
                Serial.print(softSerial.read());
                Serial.print(" ");
                Serial.println(count);
            }
        } else {
            Serial.println();
        }
    } while (softSerial.available());
}