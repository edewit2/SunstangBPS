#include "arduino.h"
#include "FrameInterface.h"
#include "TestFrame.h"

String TestFrame::toString(){
	String out = String("");
	out +=intTest;
	out+= " ";
	out += charTest;
	out += " ";
	out += int16;
	return out; 
}

int TestFrame::getLength()
{
	return 4;
}
byte* TestFrame::toByteArray(){
	byte* out = new byte[getLength()];
	out[0] = byte(intTest);
	out[1] = byte(charTest);
	out[2] = highByte(int16);
	out[3] = lowByte(int16);
	return out;
}
int TestFrame::getId(){
	return 0;
}


void TestFrame::populateProperties(byte* raw)
{
	intTest = raw[0];
	charTest = raw[1];
	int16 = word(raw[2], raw[3]);
}

TestFrame::TestFrame(uint8_t iTest, char cTest, uint16_t t)
{

	intTest = iTest;
	charTest = cTest;
	int16 = t;
}
TestFrame::TestFrame(){}
