
#define lowByte(param) param
int pinV1 = 4;
int pinV2 = 3;
int pinCurrent = 1;
int pinTemperature = 2;
enum mode { INACTIVE, COMMAND, DATA } currentMode;

int scaleVoltageOrTemp(int sensorReading) {
    //return ((sensorReading * 2) << 5) ^ 0x8000;
    return (int)(((sensorReading * 2) - 1024) << 5);
}

int scaleCurrent(int sensorReading) {
    //return (sensorReading * 64) ^ 0x8000;
    return (int) ((sensorReading * 64) - (1<<15)) ;
}


void setup() {
    Serial.begin(9600);
    currentMode = INACTIVE;
    pinMode(6, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(4, OUTPUT);

    pinMode(13, OUTPUT);

    Serial.write(0xDD);

    //while (waitForByteThenConsume() != 0xC1) {}
}

byte waitForByteThenConsume() {
    updateLed();
    while (!Serial.available()) {}
    return Serial.read();
}

byte waitForByteThenPeek() {
    updateLed();

    while (!Serial.available()) {}
    return Serial.peek();
}

void inactiveMode(byte input) {
    if (input == 0xC1) {
        Serial.write(0xCD);
        currentMode = COMMAND;
    } else {
        Serial.write(0xCF);
    }
}

void commandMode(byte command) {
    switch (command) {
    case 0xC1:
        currentMode = INACTIVE;
        break;
    case 0xE1:
        currentMode = DATA;
        break;
    }
}

void dataMode(byte data) {

    byte startAddress;
    switch (data) {
    case 0x55:

        // Device is specifying the ID
        Serial.write(0x55);

        // Echo back the next 8 bytes that follow
        for (int i = 0; i < 8; ++i) {
            Serial.write(waitForByteThenConsume());
        }
        break;
    case 0x69:

        // Read data command
        Serial.write(0x69);

        startAddress = waitForByteThenConsume();
        Serial.write(startAddress);

        // Continuously return the next piece of data for as long as bridge reads 0xFF
        while (waitForByteThenConsume() == 0xFF) {
            Serial.write(getRegisterData(startAddress++));
        }
        break;
    case 0xE3:
        // Return to command mode
        currentMode = COMMAND;
        break;
    default:
        // A byte was read that doesnt match a command. Simply echo.
        Serial.write(0xCF);

        break;
    }
}


byte getRegisterData(byte address) {
    switch (address) {
    case 0x08:		return highByte(getCurrent());
    case 0x09:		return lowByte(getCurrent());

    case 0x0A:		return highByte(getTemperature());
    case 0x0B:		return lowByte(getTemperature());

    case 0x0C:		return highByte(getV1());
    case 0x0D:		return lowByte(getV1());

    case 0x0E:		return highByte(getCurrent());
    case 0x0F:		return lowByte(getCurrent());

    case 0x1C:		return highByte(getV2());
    case 0x1D:		return lowByte(getV2());

    default:		return 0xFF;
    }
    /*switch (address) {
    case 0x08:		return 0x00;
    case 0x09:		return 0x00;

    case 0x0A:		return 0x00;
    case 0x0B:		return 0x00;

    case 0x0C:		return 0x5E;
    case 0x0D:		return 0x67;

    case 0x0E:		return 0x00;
    case 0x0F:		return 0x00;

    case 0x1C:		return 0x5E;
    case 0x1D:		return 0x67;

    default:		return 0xFF;
    }*/
}

uint16_t getCurrent() {
    return scaleCurrent(analogRead(pinCurrent));
}

uint16_t getV1() {
    return scaleVoltageOrTemp(analogRead(pinV1));
}

uint16_t getV2() {
    return scaleVoltageOrTemp(analogRead(pinV2));
}

uint16_t getTemperature() {
    return scaleVoltageOrTemp(analogRead(pinTemperature));
}

void updateLed() {

    if (currentMode == INACTIVE) {
        digitalWrite(6, HIGH);
        digitalWrite(5, LOW);
        digitalWrite(4, LOW);
    } else if (currentMode == COMMAND) {

        digitalWrite(6, LOW);
        digitalWrite(5, HIGH);
        digitalWrite(4, LOW);
    } else if (currentMode == DATA) {

        digitalWrite(6, LOW);
        digitalWrite(5, LOW);
        digitalWrite(4, HIGH);
    } else {
        digitalWrite(6, LOW);
        digitalWrite(5, LOW);
        digitalWrite(4, LOW);
    }
}

void loop() {
        ////digitalWrite(5, HIGH);
        //// Consume command
        //byte msg = waitForByteThenConsume();

        //if (currentMode == INACTIVE) {
        //    inactiveMode(msg);
        //} else if (currentMode == COMMAND) {
        //    commandMode(msg);
        //} else if (currentMode == DATA) {
        //    dataMode(msg);
        //}
    static byte a = 0;
    //Serial.write(a++);

    while (Serial.peek() != 0xC1) {
        Serial.read();
    }
    if (Serial.peek() == 0xC1) {
        Serial.read();

        uint32_t time = millis();
        while (Serial.peek() != 0xE1 && millis() - time < 300) {}

        if (Serial.peek() == 0xE1) {
            Serial.read();
            if (Serial.peek() == 0x55) {

                const byte t[] ={
                    0xCD,
                    0x55,
                    0x3D,
                    0x5B,
                    0xF3,
                    0x00,
                    0x60,
                    0x07,
                    0x00,
                    0xD9,
                    0x69,
                    0x08
                };

                for (int i = 0; i < sizeof(t)/sizeof(t[0]); i++) {
                    Serial.write(t[i]);
                }

                byte startAddress = 0x08;
                for (int i = 0; i < 22; i++) {
                    Serial.write(getRegisterData(startAddress++));
                }

                Serial.write(0xCD);
            }
        }
    }
}



//uint64_t id = (uint64_t)response[2];
/*
if (id != 0x3D5BF300600700D9ULL
&& id != 0x3D0BF000600700B9ULL
&& id != 0x3D07F000600700C4ULL
&& id != 0x3DE2EF0060070015ULL
&& id != 0x3D27EF0060070061ULL
&& id != 0x3DD9EE00600700CDULL
&& id != 0x3D50EF006007007CULL
&& id != 0x3DC3EF0060070094ULL
&& id != 0x3DA7F300600700D6ULL
&& id != 0x3DDCEE0060070026ULL
&& id != 0x3D9DF20060070039ULL
&& id != 0x3DE1EF006007004CULL
&& id != 0x3D98EF0060070042ULL
&& id != 0x3Db9F1006007001DULL
&& id != 0x3D53EF0060070025ULL
&& id != 0x3D97EF0060070066ULL
&& id != 0x3DC0EF00600700CDULL
&& id != 0x3DE0EF006007007BULL
&& id != 0x3D2CEF0060070099ULL
&& id != 0x3DC8F200600700FCULL
&& id != 0x3DDEEF0060070085ULL) return;
*/
