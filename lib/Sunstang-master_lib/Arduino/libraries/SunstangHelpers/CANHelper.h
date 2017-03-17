#define CAN_SPEED CAN_1000KBPS

uint32_t get32BitUint(uint8_t start, uint8_t in[])
{
  return  (uint32_t)(in[start + 0]) << 24 | (uint32_t)(in[start + 1]) << 16 | (uint32_t)(in[ start + 2]) << 8 | (uint32_t)(in[ start + 3]);
}

void insert32Bit(uint8_t start, uint8_t buffer[], uint32_t value)
{
  buffer[start + 0] = (value >> 24) & 0xFF;
  buffer[start + 1] = (value >> 16) & 0xFF;
  buffer[start + 2] = (value >>  8) & 0xFF;
  buffer[start + 3] = (value >>  0) & 0xFF;
}