#ifndef TESTFRAME_h
#define TESTFRAME_h

#include "arduino.h"
#include "FrameInterface.h"

class TestFrame: public FrameInterface {
	uint8_t intTest;
	char charTest;
	uint16_t int16;
	public:
		TestFrame();
		TestFrame(uint8_t, char, uint16_t);
		virtual void populateProperties(byte*);
		virtual int getLength();
		virtual byte* toByteArray();
		virtual int getId();
		virtual String toString();
};
#endif