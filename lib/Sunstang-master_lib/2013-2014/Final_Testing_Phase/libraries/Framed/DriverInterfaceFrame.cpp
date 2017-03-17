#include "arduino.h"
#include "FrameInterface.h"
#include "DriverInterfaceFrame.h"

String DriverInterfaceFrame::toString(){
	String out = String("");
	out +=buttons;
	out+= ",";
	out += speed;
	out += ",";
	out += temp;
	return out; 
}

int DriverInterfaceFrame::getLength()
{
	return 4;
}
byte* DriverInterfaceFrame::toByteArray(){
	byte* out = new byte[getLength()];
	out[0] = byte(buttons);
	out[1] = byte(speed);
	out[2] = highByte(temp);
	out[3] = lowByte(temp);
	return out;
}
int DriverInterfaceFrame::getId(){
	return 1;
}


void DriverInterfaceFrame::populateProperties(byte* raw)
{
	buttons = raw[0];
	speed = raw[1];
	temp = word(raw[2], raw[3]);
}

DriverInterfaceFrame::DriverInterfaceFrame(uint8_t b, uint8_t s, uint16_t t)
{

	buttons = b;
	speed = s;
	temp = t;
}
DriverInterfaceFrame::DriverInterfaceFrame(){}
