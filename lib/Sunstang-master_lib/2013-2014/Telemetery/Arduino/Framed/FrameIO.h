#ifndef FRAMEIO_h
#define FRAMEIO_h

#include "Arduino.h"
#include "FrameInterface.h"

class FrameIO{

	static const byte startFlag;
	static const byte endFlag;
	static const byte escapeFlag;

	private:
		bool escape(byte in);
	public:
		FrameIO();
		FrameInterface* readDataFrame();
		void writeDataFrame(byte[], byte, int);
		void writeDataFrame(FrameInterface*);

};
#endif