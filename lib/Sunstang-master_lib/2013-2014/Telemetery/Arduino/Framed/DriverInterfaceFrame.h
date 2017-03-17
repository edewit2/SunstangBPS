#ifndef DRIVERINTERFACEFRAME_h
#define DRIVERINTERFACEFRAME_h

#include "arduino.h"
#include "FrameInterface.h"

class DriverInterfaceFrame: public FrameInterface {
	uint8_t buttons;
	uint8_t speed;
	uint16_t temp;
	public:
		DriverInterfaceFrame();
		DriverInterfaceFrame(uint8_t, uint8_t, uint16_t);
		virtual void populateProperties(byte*);
		virtual int getLength();
		virtual byte* toByteArray();
		virtual int getId();
		virtual String toString();
};
#endif