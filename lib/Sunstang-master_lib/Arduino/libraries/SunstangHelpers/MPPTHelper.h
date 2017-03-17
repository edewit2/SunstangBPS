#define MPPT_1_REQUEST_CAN_ID 0x711
#define MPPT_1_RESPONSE_CAN_ID 0x771

#define MPPT_2_REQUEST_CAN_ID 0x712
#define MPPT_2_RESPONSE_CAN_ID 0x772

bool getBatteryLevelReached(uint8_t buffer[])
{
  return bitRead(buffer[0], 7);
}

bool getOverTempFlag(uint8_t buffer[])
{
  return bitRead(buffer[0], 6);
}

bool getNoChargeFlagFlag(uint8_t buffer[])
{
  return bitRead(buffer[0], 5);
}

bool getUnderVoltageFlag(uint8_t buffer[])
{
  return bitRead(buffer[0], 4);
}

uint16_t getVoltageIn(uint8_t buffer[])
{
  return (uint16_t)(buffer[0] & 0x3) << 8  | buffer[1];
}

uint16_t getCurrentIn(uint8_t buffer[])
{
  return (uint16_t)(buffer[2] & 0x3) << 8  | buffer[3];
}

uint16_t getVoltageOut(uint8_t buffer[])
{
  return (uint16_t)(buffer[4] & 0x3) << 8  | buffer[5];
}

uint8_t getMPPTTemp(uint8_t buffer[])
{
  return buffer[6];
} 