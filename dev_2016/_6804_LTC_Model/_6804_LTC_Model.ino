class LTC6804Stack {
public:
  LTC6804Stack()
  {
    initLocalRegisters();
  }

  void wakeUpSerialInterface()
  {
    // 1. Send a dummy byte. The activity on CSB and SCK will wake up the serial interface
    digitalWrite(chipSelectPin, LOW);
    transfer(0x00);
    digitalWrite(chipSelectPin, HIGH);

    // 2. For n devices in the stack, wait for the amount of time n * t_wake in order to power up all the devices (t_wake = 300us)
    delayMicroseconds(nDevices * 300);
  }
  void initVoltageSensorSPI()
  {
    //pinMode(slaveSelectVoltageSensor, OUTPUT);

    //// Need to do a couple things before communication
    //// 1. Set the bit order
    //// 2. Set data clock idle when high/low and set samples on rising or falling clk edges
    //// 3. Speed of SPI

    //SPI.setBitOrder(MSBFIRST);

    //// LTC6804 Datasheet Pg. 35, "Timing": CPHA = 1 and CPOL = 1
    //SPI.setDataMode(SPI_MODE3);

    //// The maximum data rate for the LTC6804 using a CAT 5 twisted pair cable up to 10 meters is 1Mbps 
    //// (see Datasheet Pg. 35, "Timing" and pg 70, "Applications Information")
    //// Arduino Nanos have a clock speed of 16 MHz. Using a clock divider of 16 will result in a SPI clock speed of 1MHz, which transfers 1Mbps
    //SPI.setClockDivider(SPI_CLOCK_DIV16);
  }
  // WRCFG
  void writeConfigurationRegisterGroup()
  {
    // Send the command
    sendCommandAndPec(0x00, 0x01);

    // send the data
    for (char device = nDevices-1; device >= 0; device--)
    {
      // Send the same configuration to all devices, starting with the highest stacked device
      sendBytesAndPEC(CFGR[device], 6);
    }
  }
  void readCellVoltageRegisterGroupA()
  {
    sendCommandAndPec(0x00, 0x04);
    readBytesIntoRegister(CVAR);
  }
  void readCellVoltageRegisterGroupB()
  {
    sendCommandAndPec(0x00, 0x06);
    readBytesIntoRegister(CVBR);
  }
  void readCellVoltageRegisterGroupC()
  {
    sendCommandAndPec(0x00, 0x08);
    readBytesIntoRegister(CVCR);
  }
  void readCellVoltageRegisterGroupD()
  {
    sendCommandAndPec(0x00, 0x0A);
    readBytesIntoRegister(CVDR);
  }
  float getCellVoltageInVolts(char absoluteCellNumber)
  {
    return getCellVoltageRaw(absoluteCellNumber) * 0.0001;
  }
  unsigned short getCellVoltageRaw(char absoluteCellNumber)
  {
    // Cell number goes from 1 to 12*nCells
    // First figure out which device the cell exists on

    char deviceNumber = absoluteCellNumber / 12;
    char relativeCellNumber = (absoluteCellNumber-1) % 12 + 1;

    switch (relativeCellNumber)
    {
    // Cell Voltage Register A
    case 1:
      return word(CVAR[deviceNumber][1], CVAR[deviceNumber][0]);
    case 2:
      return word(CVAR[deviceNumber][3], CVAR[deviceNumber][2]);
    case 3:
      return word(CVAR[deviceNumber][5], CVAR[deviceNumber][4]);
      
    // Cell Voltage Register B
    case 4:
      return word(CVBR[deviceNumber][1], CVBR[deviceNumber][0]);
    case 5:
      return word(CVBR[deviceNumber][3], CVBR[deviceNumber][2]);
    case 6:
      return word(CVBR[deviceNumber][5], CVBR[deviceNumber][4]);

    // Cell Voltage Register C
    case 7:
      return word(CVCR[deviceNumber][1], CVCR[deviceNumber][0]);
    case 8:
      return word(CVCR[deviceNumber][3], CVCR[deviceNumber][2]);
    case 9:
      return word(CVCR[deviceNumber][5], CVCR[deviceNumber][4]);

    // Cell Voltage Register D
    case 10:
      return word(CVDR[deviceNumber][1], CVDR[deviceNumber][0]);
    case 11:
      return word(CVDR[deviceNumber][3], CVDR[deviceNumber][2]);
    case 12:
      return word(CVDR[deviceNumber][5], CVDR[deviceNumber][4]);

      break;

    default:
      // Error
      return 0xFF;
    }

  }

private:
  // Stacked devices are numbered from 0 (the bottom) to nDevices-1 (the top)
  static const unsigned char nDevices = 1;
  static const unsigned char chipSelectPin = 9;
  unsigned char CFGR[nDevices][6];
  unsigned char CVAR[nDevices][6];
  unsigned char CVBR[nDevices][6];
  unsigned char CVCR[nDevices][6];
  unsigned char CVDR[nDevices][6];
  unsigned char STAR[nDevices][6];
  unsigned char STBR[nDevices][6];

  // Built-in Arduino functions
  unsigned char transfer(unsigned char b)
  {
    static unsigned char currentIndex = 0;
    buffer[currentIndex] = b;
    currentIndex++;
    return 0xAA;
  }
  static void bitWrite(short &x, unsigned char n, unsigned char b)
  {
    if (b == 0)
    {
      // Clear: AND
      x = x & ~(0x0001 << n);
    }
    else
    {
      // Set: OR
      x = x | (0x0001 << n);
    }
  }
  static short bitRead(short x, unsigned char n)
  {
    return 0x0001 & (x >> n);
  }
  static unsigned char lowByte(short input)
  {
    return input & 0xFF;
  }
  static unsigned char highByte(short input)
  {
    return input >> 8 & 0xFF;
  }
  static short word(unsigned char high, unsigned char low)
  {
    return low | (high << 8);
  }
  enum pinState
  {
    HIGH,
    LOW
  };
  static void digitalWrite(char pin, pinState state)
  {

  }
  static void delayMicroseconds(unsigned int delay)
  {

  }
  // End built-in arduino functions

  void initLocalRegisters()
  {
    for (unsigned char device = 0; device < nDevices; device++)
    {
      CFGR[device][0] = 0xD8;
      CFGR[device][1] = 0xD5;
      CFGR[device][2] = 0x16;
      CFGR[device][3] = 0xA4;
      CFGR[device][4] = 0x00;
      CFGR[device][5] = 0x00;

      for (unsigned char i = 0; i < 6; i++)
      {
        CVAR[device][i] = 0xFF;
        CVBR[device][i] = 0xFF;
        CVCR[device][i] = 0xFF;
        CVDR[device][i] = 0xFF;
        STAR[device][i] = 0xFF;
        STBR[device][i] = 0xFF;
      }
    }
  }

  // This algorithm is described on page 42 of the 6804 Datasheet
  static short calculatePEC(unsigned char* DIN, unsigned char nBytes)
  {
    // 1.Initialize the PEC to 0x0010
    short PEC = 0x0010;

    // 2. For each bit DIN coming into the PEC register group
    for (char byteNumber = 0; byteNumber < nBytes; byteNumber++)
    {
      for (char bitNumber = 7; bitNumber >= 0; bitNumber--)
      {
        // Set IN
        short IN = 0x0000;
        bitWrite(IN, 0, bitRead(DIN[byteNumber], bitNumber) ^ bitRead(PEC, 14));
        bitWrite(IN, 3, bitRead(IN, 0) ^ bitRead(PEC, 2));
        bitWrite(IN, 4, bitRead(IN, 0) ^ bitRead(PEC, 3));
        bitWrite(IN, 7, bitRead(IN, 0) ^ bitRead(PEC, 6));
        bitWrite(IN, 8, bitRead(IN, 0) ^ bitRead(PEC, 7));
        bitWrite(IN, 10, bitRead(IN, 0) ^ bitRead(PEC, 9));
        bitWrite(IN, 14, bitRead(IN, 0) ^ bitRead(PEC, 13));

        // Update 15-bit PEC
        PEC = PEC << 1;
        bitWrite(PEC, 14, bitRead(IN, 14));
        bitWrite(PEC, 10, bitRead(IN, 10));
        bitWrite(PEC, 8, bitRead(IN, 8));
        bitWrite(PEC, 7, bitRead(IN, 7));
        bitWrite(PEC, 4, bitRead(IN, 4));
        bitWrite(PEC, 3, bitRead(IN, 3));
        bitWrite(PEC, 0, bitRead(IN, 0));
      }
    }

    // 4. Append at the LSB then return
    return PEC << 1;
  }

  // Sends the data followed by the PEC on the SPI line
  void sendBytesAndPEC(unsigned char* data, unsigned char length)
  {
    for (char i = 0; i < length; i++)
    {
      transfer(data[i]);
    }

    short pec = calculatePEC(data, length);
    transfer(highByte(pec));
    transfer(lowByte(pec));
  }

  // Sends a command followed by its pec
  void sendCommandAndPec(unsigned char byte0, unsigned char byte1)
  {
    const unsigned char length = 2;
    unsigned char* bytes = new unsigned char[length];
    bytes[0] = byte0;
    bytes[1] = byte1;
    sendBytesAndPEC(bytes, length);
    delete[] bytes;
  }

  // Reads the following bytes into the registers
  void readBytesIntoRegister(unsigned char (&registerGroup)[nDevices][6])
  {
    // Response begins with registers in lowest device in stack
    for (unsigned char deviceNumber = 0; deviceNumber < nDevices; deviceNumber++)
    {
      // Registers arrive starting with reg0, reg1, reg2... reg5
      for (unsigned char reg = 0; reg < 6; reg++)
      {
        char response = transfer(0x00);
        registerGroup[deviceNumber][reg] = response;
      }
    }
  }
};

