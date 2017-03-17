#define GPS_SPEED_COURSE_CAN_ID 0x2A1
#define GPS_LOCATION_CAN_ID 0x2A0
#define GPS_ALTITUDE_TIME_CAN_ID 0x2A2

uint32_t getSpeed(uint8_t in[])
{
  return get32BitUint(0, in);
}

uint32_t getDirection(uint8_t in[])
{
  return get32BitUint(4, in);
}

uint32_t getLatitiude(uint8_t in[])
{
  return get32BitUint(0, in);
}

uint32_t getLongitude(uint8_t in[])
{
  return get32BitUint(4, in);
}

uint32_t getAltitude(uint8_t in[])
{
  return get32BitUint(0, in);
}

uint32_t getTime(uint8_t in[])
{
  return get32BitUint(4, in);
}
