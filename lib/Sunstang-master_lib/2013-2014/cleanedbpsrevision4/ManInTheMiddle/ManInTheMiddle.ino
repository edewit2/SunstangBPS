void setup() {
    Serial.begin(9600);
}

byte waitForByte() {
    while (!Serial.available()) {}
    return Serial.read();
}

void loop() {
    if (waitForByte() == 0x69) { // 0xCD
        if (waitForByte() == 0x08) { // 0x55
            //Serial.println("BEGINNING");
            //Serial.print(0xCD, HEX); Serial.print(" ");
            //Serial.print(0x55, HEX); Serial.print(" ");
            byte b;
            //for (int i = 0; i < 35 - 3 - 22; i++) {
            //    b = waitForByte();
            //    if (b <= 0x0F) {
            //        Serial.print("0");
            //        Serial.print(b, HEX);
            //    } else {
            //        Serial.print(b, HEX);
            //    }
            //    Serial.print(" ");
            //}

            for (int i = 0; i < 11; i++) {
                b = waitForByte();
                if (b <= 0x0F) {
                    Serial.print("0");
                    Serial.print(b, HEX);
                } else {
                    Serial.print(b, HEX);
                }
                b = waitForByte();
                if (b <= 0x0F) {
                    Serial.print("0");
                    Serial.print(b, HEX);
                } else {
                    Serial.print(b, HEX);
                }
                Serial.print(" ");
            }

            //Serial.println("CD");

            Serial.println();
            //Serial.println("ENDING");
            Serial.println();
        }
    }
    //if (waitForByte() == 0xFE) {
    //    Serial.println("TIMEOUT");
    //}
}
