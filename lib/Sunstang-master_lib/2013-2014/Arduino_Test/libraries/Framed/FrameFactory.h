#ifndef FRAMEFACTORY_h
#define FRAMEFACTORY_h

#include "FrameInterface.h"
#include "Arduino.h"

class FrameFactory{
	public:
		static FrameInterface* createFrame(byte type);

};
#endif