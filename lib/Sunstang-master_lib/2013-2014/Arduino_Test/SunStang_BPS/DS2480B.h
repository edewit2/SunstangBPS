#ifndef DS2480B_H
#define DS2480B_H

class DS2480B {
private:
	Stream* serial;

public:
	DS2480B() {
		// empty
	}

	static void attachSerial(Stream* serial) {
		serial = serial;
	}
	
	uint64_t getId() {
		serial->write(0xE1); //Tell bridge to enter data mode
		serial->write(0x33); //Command to 1-wire device for Read NET ID
		serial->write(0xFF); //buffer for data return
		serial->write(0xFF); //buffer for data return
		serial->write(0xFF); //buffer for data return
		serial->write(0xFF); //buffer for data return
		serial->write(0xFF); //buffer for data return  
		serial->write(0xFF); //buffer for data return
		serial->write(0xFF); //buffer for data return
		serial->write(0xFF); //buffer for data return
		serial->write(0xE3); //Tell bridge to enter command mode (need to be in command mode to send a reset pulse)
		serial->write(0xC1); //Tell Bridge to send reset pulse to 1-wire bus i.e. end of tx
	}

	/*
	READ MEMORY SEQUENCE
	Action Sequence         Host TX    Host RX
	----------------------------------------------
	Generate Reset Pulse    C1         CD or ED
	Set Data Mode           E1         (nothing)
	Skip ROM Command        CC         CC
	Read Memory Command     F0         (as sent)
	Starting Address TA1    40         (as sent)
	Starting Address TA2    00         (as sent)
	Read 8 Bytes of Data    FF         (x8) (data)
	Set Command Mode        E3         (nothing)
	Generate Reset Pulse    C1         CD or ED
	*/
	bool readData(uint8_t command, int16_t& data) {
		serial->write(0xC1);
		serial->write(0xE1);
		serial->write(0xCC); //skip rom command
		//serial->write(0x69);  //read memory command
		serial->write(command); // MSB of the item you with to recieve.
		serial->write(command+1);
		serial->write(0xFF);
		serial->write(0xE3);
		serial->write(0xC1);
	}
};

#endif