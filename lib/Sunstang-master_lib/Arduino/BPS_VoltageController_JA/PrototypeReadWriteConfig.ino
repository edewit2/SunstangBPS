#include <SPI.h>


class LTC6804Stack {
public:
	LTC6804Stack()
	{
		resetLocalRegisters();
	}

	void resetLocalRegisters()
	{
		for (byte device = 0; device < nDevices; device++)
		{
			CFGR[device][0] = 0xD8;
			CFGR[device][1] = 0xD5;
			CFGR[device][2] = 0x16;
			CFGR[device][3] = 0xA4;
			CFGR[device][4] = 0x00;
			CFGR[device][5] = 0x00;

			for (byte i = 0; i < 6; i++)
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

	void wakeUpSerialInterface()
	{
		Serial.println("\nWake up SPI");
		delayMicroseconds(1); // Minimum length of time of SCK low is 0.2us

		// 1. Send a dummy byte. The activity on CSB and SCK will wake up the serial interface
		digitalWrite(chipSelectPin, LOW);
		debugTransfer(0x00);
		digitalWrite(chipSelectPin, HIGH);

		// 2. For n devices in the stack, wait for the amount of time n * t_wake in order to power up all the devices (t_wake = 300us)
		delayMicroseconds(nDevices * 300);
	}

	void initVoltageSensorSPI()
	{
		pinMode(chipSelectPin, OUTPUT);
		SPI.begin();

		// Need to do a couple things before communication
		// 1. Set the bit order
		// 2. Set data clock idle when high/low and set samples on rising or falling clk edges
		// 3. Speed of SPI

		SPI.setBitOrder(MSBFIRST);

		// LTC6804 Datasheet Pg. 35, "Timing": CPHA = 1 and CPOL = 1
		SPI.setDataMode(SPI_MODE3);

		// The maximum data rate for the LTC6804 using a CAT 5 twisted pair cable up to 10 meters is 1Mbps 
		// (see Datasheet Pg. 35, "Timing" and pg 70, "Applications Information")
		// Arduino Nanos have a clock speed of 16 MHz. Using a clock divider of 16 will result in a SPI clock speed of 1MHz, which transfers 1Mbps
		SPI.setClockDivider(SPI_CLOCK_DIV16);
	}

	// WRCFG
	void writeConfigurationRegisterGroup()
	{
		Serial.println("\nWrite Config");
		delayMicroseconds(1); // Minimum length of time of SCK low is 0.2us
		// Pull CSB low
		digitalWrite(chipSelectPin, LOW);

		// Send the command
		sendCommandAndPec(0x00, 0x01);

		// send the data
		for (short device = nDevices - 1; device >= 0; device--)
		{
			// Send the same configuration to all devices, starting with the highest stacked device
			sendBytesAndPEC(CFGR[device], 6);
		}

		// Pull CSB high, data is latched on rising edge
		digitalWrite(chipSelectPin, HIGH);
	}
	// RDCFG
	void readConfigurationRegisterGroup()
	{
		Serial.println("\nRead RDCFG");
		delayMicroseconds(1); // Minimum length of time of SCK low is 0.2us
		digitalWrite(chipSelectPin, LOW);
		sendCommandAndPec(0x00, 0x02);
		readBytesIntoRegister(CFGR);
		digitalWrite(chipSelectPin, HIGH);
	}
	void readCellVoltageRegisterGroupA()
	{
		Serial.println("\nRead CVAR");
		delayMicroseconds(1); // Minimum length of time of SCK low is 0.2us
		digitalWrite(chipSelectPin, LOW);
		sendCommandAndPec(0x00, 0x04);
		readBytesIntoRegister(CVAR);
		digitalWrite(chipSelectPin, HIGH);
	}
	void readCellVoltageRegisterGroupB()
	{
		Serial.println("\nRead CVBR");
		delayMicroseconds(1); // Minimum length of time of SCK low is 0.2us
		digitalWrite(chipSelectPin, LOW);
		sendCommandAndPec(0x00, 0x06);
		readBytesIntoRegister(CVBR);
		digitalWrite(chipSelectPin, HIGH);
	}
	void readCellVoltageRegisterGroupC()
	{
		Serial.println("\nRead CVCR");
		delayMicroseconds(1); // Minimum length of time of SCK low is 0.2us
		digitalWrite(chipSelectPin, LOW);
		sendCommandAndPec(0x00, 0x08);
		readBytesIntoRegister(CVCR);
		digitalWrite(chipSelectPin, HIGH);
	}
	void readCellVoltageRegisterGroupD()
	{
		Serial.println("\nRead CVDR");
		delayMicroseconds(1); // Minimum length of time of SCK low is 0.2us
		digitalWrite(chipSelectPin, LOW);
		sendCommandAndPec(0x00, 0x0A);
		readBytesIntoRegister(CVDR);
		digitalWrite(chipSelectPin, HIGH);
	}
	void startCellVoltageADCConversion()
	{
		Serial.println("\nStart ADC");
		delayMicroseconds(1); // Minimum length of time of SCK low is 0.2us
		digitalWrite(chipSelectPin, LOW);
		sendCommandAndPec(0x03, 0x70);
		digitalWrite(chipSelectPin, HIGH);

	}
	float getCellVoltageInVolts(byte absoluteCellNumber)
	{
		return getCellVoltageRaw(absoluteCellNumber) * 0.0001;
	}
	unsigned short getCellVoltageRaw(byte absoluteCellNumber)
	{
		// Cell number goes from 0 to 12*nCells-1
		// First figure out which device the cell exists on

		char deviceNumber = absoluteCellNumber / 12;
		char relativeCellNumber = absoluteCellNumber % 12;

		switch (relativeCellNumber)
		{
			// Cell Voltage Register A
		case 0:
			return word(CVAR[deviceNumber][1], CVAR[deviceNumber][0]);
		case 1:
			return word(CVAR[deviceNumber][3], CVAR[deviceNumber][2]);
		case 2:
			return word(CVAR[deviceNumber][5], CVAR[deviceNumber][4]);

			// Cell Voltage Register B
		case 3:
			return word(CVBR[deviceNumber][1], CVBR[deviceNumber][0]);
		case 4:
			return word(CVBR[deviceNumber][3], CVBR[deviceNumber][2]);
		case 5:
			return word(CVBR[deviceNumber][5], CVBR[deviceNumber][4]);

			// Cell Voltage Register C
		case 6:
			return word(CVCR[deviceNumber][1], CVCR[deviceNumber][0]);
		case 7:
			return word(CVCR[deviceNumber][3], CVCR[deviceNumber][2]);
		case 8:
			return word(CVCR[deviceNumber][5], CVCR[deviceNumber][4]);

			// Cell Voltage Register D
		case 9:
			return word(CVDR[deviceNumber][1], CVDR[deviceNumber][0]);
		case 10:
			return word(CVDR[deviceNumber][3], CVDR[deviceNumber][2]);
		case 11:
			return word(CVDR[deviceNumber][5], CVDR[deviceNumber][4]);

			break;

		default:
			// Error
			return 0xFF;
		}

	}
	void readStatusRegisterGroupA()
	{
		Serial.println("\nRead STAR");
		delayMicroseconds(1); // Minimum length of time of SCK low is 0.2us
		digitalWrite(chipSelectPin, LOW);
		sendCommandAndPec(0x00, 0x10);
		readBytesIntoRegister(STAR);
		digitalWrite(chipSelectPin, HIGH);
	}
	void readStatusRegisterGroupB()
	{
		Serial.println("\nRead STBR");
		delayMicroseconds(1); // Minimum length of time of SCK low is 0.2us
		digitalWrite(chipSelectPin, LOW);
		sendCommandAndPec(0x00, 0x12);
		readBytesIntoRegister(STBR);
		digitalWrite(chipSelectPin, HIGH);

                // Check to ensure for new model board. Check bit 3 of STBR5
                for (byte boardNumber = 0; boardNumber < nDevices; boardNumber++)
                {
                    if (STBR[boardNumber][5] & B00001000 == B00000000)
                    {
                        Serial.print("WARNING: Board ");
                        Serial.print(boardNumber);
                        Serial.print(" is prone to WRCOMM/WRCFG failures!");
                    }
                }
                
	}
        byte getRSVD(byte boardNumber)
        {
                // Returns 0 if the board is the old model, 1 if the new model.
                return STBR[boardNumber][5] && 0x08;
        }

private:
	// Stacked devices are referenced from 0 (the bottom) to nDevices-1 (the top)
	static const byte nDevices = 1;			// This number determines the number of 6804's that are stacked
	static const byte chipSelectPin = 9;	// This number determines the chipSelect for the LTC6804 SPI connection
	byte CFGR[nDevices][6];
	byte CVAR[nDevices][6];
	byte CVBR[nDevices][6];
	byte CVCR[nDevices][6];
	byte CVDR[nDevices][6];
	byte STAR[nDevices][6];
	byte STBR[nDevices][6];

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
	void sendBytesAndPEC(byte* data, unsigned char length)
	{
		for (char i = 0; i < length; i++)
		{
			debugTransfer(data[i]);
		}

		short pec = calculatePEC(data, length);
		debugTransfer(highByte(pec));
		debugTransfer(lowByte(pec));
	}

	// Sends a command followed by its pec
	void sendCommandAndPec(byte byte0, byte byte1)
	{
		byte bytes[2];
		bytes[0] = byte0;
		bytes[1] = byte1;
		sendBytesAndPEC(bytes, 2);
	}

	// Reads the following bytes into the registers
	void readBytesIntoRegister(byte(&registerGroup)[nDevices][6])
	{
		// Response begins with registers in lowest device in stack
		for (byte deviceNumber = 0; deviceNumber < nDevices; deviceNumber++)
		{
			// Registers arrive starting with reg0, reg1, reg2... reg5
			for (byte reg = 0; reg < 6; reg++)
			{
				byte response = debugTransfer(0x00);
				registerGroup[deviceNumber][reg] = response;
			}

			// Check following PEC 
			short responsePec = calculatePEC(registerGroup[deviceNumber], 6);
			byte pec0 = debugTransfer(0x00);
			byte pec1 = debugTransfer(0x00);
			if (highByte(responsePec) != pec0 || lowByte(responsePec) != pec1)
			{
				Serial.println("PEC from device does not match locally calculated pec");
			}
		}
	}
};


void setup() {
  
  
		Serial.begin(115200);

		// Init comm
		LTC6804Stack stack;
		stack.initVoltageSensorSPI();

		// Wake up serial
		stack.wakeUpSerialInterface(); // TODO: Integrate this automatically in all functions

		// Read Config
		stack.readConfigurationRegisterGroup();

		// Write Config
		stack.resetLocalRegisters();
		stack.writeConfigurationRegisterGroup();

		// Read Config
		stack.readConfigurationRegisterGroup();

		//stack.readCellVoltageRegisterGroupA();

		stack.startCellVoltageADCConversion();

		// Read Voltage Register Group A
		stack.readCellVoltageRegisterGroupA();
		stack.readCellVoltageRegisterGroupB();
		stack.readCellVoltageRegisterGroupC();
	        stack.readCellVoltageRegisterGroupD();
    
		for (byte cell = 0; cell < 36; cell++)
		{
			Serial.print("Cell ");
			Serial.print(cell);
			Serial.print(": ");
			Serial.print(stack.getCellVoltageInVolts(cell));
			Serial.println("V");
		}
                // By now, the serial interface would have went back to sleep. Need to wake
                stack.wakeUpSerialInterface();
                
                Serial.println("\nChecking Status");
		stack.readCellVoltageRegisterGroupB();
                Serial.println(stack.getRSVD(1));
                stack.readStatusRegisterGroupA();
                stack.readStatusRegisterGroupB();
}

byte debugTransfer(byte b)
{
  Serial.print("Sending: \t");
  Serial.print(b, HEX);
  Serial.print("\tReceiving: \t");
  byte response = SPI.transfer(b);
  Serial.println(response, HEX);
  return response; 
}
void loop() {
  
}
