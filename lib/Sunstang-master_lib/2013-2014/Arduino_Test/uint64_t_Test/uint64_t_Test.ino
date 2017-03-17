void setup() {
    Serial.begin(9600);
}

uint32_t a = 0xDEADBEEF;
uint32_t b = 0xC0DEF00D;

uint64_t c = (a << 32) + b;

void loop()
{
    Serial.write(a);
    Serial.write(b);
    Serial.write((uint32_t)c >> 32);
    Serial.write((uint32_t)c);
    delay(5000);
}
