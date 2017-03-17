#include "FrameInterface.h"
#include "FrameFactory.h"
#include "TestFrame.h"
#include "DriverInterfaceFrame.h"

FrameInterface* FrameFactory::createFrame(byte type){
	FrameInterface* out;

	if(type == 0)
		out =new TestFrame();
	else if(type == 1)  //This is a driverinterface frame
		out = new DriverInterfaceFrame();
	return out;
}