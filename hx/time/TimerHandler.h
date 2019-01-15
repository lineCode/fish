#ifndef TIMERHANDLER_H
#define TIMERHANDLER_H
#include <stdint.h>

class TimeoutHandler {
	friend class TimerWheelMgr;
	friend class TimerWheel;
	friend class TimerHeap;
public:
	TimeoutHandler() {}

	virtual ~TimeoutHandler() {}

	virtual int HandleTimeout() {
		return 0;
	}

private:
	uint64_t expire_;
	int timeout_;
	int id_;
};

#endif