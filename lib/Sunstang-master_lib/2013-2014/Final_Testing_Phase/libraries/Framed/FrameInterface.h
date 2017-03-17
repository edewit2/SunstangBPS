#ifndef FRAMEINTERFACE_h
#define FRAMEINTERFACE_h

#include "arduino.h"

class FrameInterface{
	public:
		virtual void populateProperties(byte*)=0;
		virtual byte* toByteArray()=0;
		virtual int getLength()=0;
		virtual int getId()=0;
		virtual String toString()=0;
};
#endif