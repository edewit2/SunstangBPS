#include "Arduino.h"
#include "FrameIO.h"
#include "FrameInterface.h"
#include "FrameFactory.h"

const byte FrameIO::startFlag = 83;
const byte FrameIO::endFlag = 69;
const byte FrameIO::escapeFlag = 88;

FrameInterface* FrameIO::readDataFrame(){
	while(Serial.read() != FrameIO::startFlag){} //Block until we find the start flag
	//Serial.println("Unblocked");
	while(!Serial.available()){}
	FrameInterface* frame = FrameFactory::createFrame(Serial.read()); //Create a frame based on the type of frame;
	//Serial.println("Made Frame");
	bool valid = true;
	byte* rawData;
	byte check =0, recCheck = 0;

	if(frame){ //make sure the frame type is valid
		rawData = new byte[frame->getLength()];
		bool escaped = false;
		byte curr =-1;

		//Serial.print("Length");
		//Serial.println(frame->getLength());

		for(int i=0; i<frame->getLength() && valid; i++){
			while(!Serial.available()){}
			
			curr =  Serial.read();

			//Serial.print("curr: ");
			//Serial.println(curr);

			if(escaped){	//if we've been escaped, read no matter what
				rawData[i] = curr;
				escaped = false;
				check = check ^ curr;
			}else if(!escaped && curr == FrameIO::escapeFlag){//if we need to escape the next character
				escaped = true;
				i--;
			}else if(!escaped && curr == FrameIO::endFlag)//We are finished, but we shouldn't be... Yikes
				valid = false;
			else{ //Check the normal case
				rawData[i] = curr;
				check = check ^ curr;
			}
		}
		while(!Serial.available()){}
		recCheck = Serial.read();
		while(!Serial.available()){}
		curr = Serial.read();
		if(curr != FrameIO::endFlag)
			valid = false;
	}else{
		valid = false;
	}

	if(valid && check == recCheck){	//confirm our checksum's are the same
		frame->populateProperties(rawData);
		return frame;
	}else
		return NULL;

}

FrameIO::FrameIO(){}

bool FrameIO::escape(byte in)
{
	if(in == FrameIO::startFlag || in == FrameIO::endFlag || in == FrameIO::escapeFlag)
	{
		Serial.write(FrameIO::escapeFlag);
		return true;
	}
}

void FrameIO::writeDataFrame(FrameInterface* f){
	this->writeDataFrame(f->toByteArray(), f->getId(), f->getLength());
}

void FrameIO::writeDataFrame(byte data[], byte type, int len){
	byte checksum = 0;
	Serial.write(FrameIO::startFlag);

	escape(type);
	Serial.write(type);

	for(int i = 0; i<len; i++){
		escape(data[i]);
		Serial.write(data[i]);
		checksum = checksum^data[i];
	}
	Serial.write(checksum);
	Serial.write(FrameIO::endFlag);      //End Byte
}

void FrameIO::writeDataFrame(uint8_t nodeIndex, uint8_t packetIndex, uint8_t* packet, uint8_t len){
	byte checksum = 0;
	Serial.write(FrameIO::startFlag);

	escape(nodeIndex);
	Serial.write(nodeIndex);

	escape(packetIndex);
	Serial.write(packetIndex);

	for(int i = 0; i<len; i++){
		escape(packet[i]);
		Serial.write(packet[i]);
		checksum = checksum^packet[i];
	}
	Serial.write(checksum);
	Serial.write(FrameIO::endFlag);      //End Byte
}