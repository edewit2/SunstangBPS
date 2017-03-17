#ifndef TIMED_FUNCTION_H
#define TIMED_FUNCTION_H

#include "inttypes.h"

class TimedFunction {
public:
	uint16_t updateTime;
	uint32_t timeLast;
	/* array of size two, holding pointers to functions taking zero
	   arguments and returning nothing
	*/
	void (*pFunction)();
	void (*const pFuncHold)();

	TimedFunction(void(*func)(), uint16_t _updateTime) :
		updateTime(_updateTime), pFuncHold(func), timeLast(0) {
		pFunction = &fnNull;
	}

	void start() {
		pFunction = pFuncHold;
	}

	void stop() {
		pFunction = &fnNull;
	}

	void update(uint32_t& timeNow) {
		if (timeNow - timeLast > updateTime) {
			(*pFunction)();
			timeLast = timeNow;
		}
	}

private:
	static void fnNull() {}
};

#endif