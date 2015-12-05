#ifndef TIMERHANDLER_H
#define TIMERHANDLER_H
#include "../Typedef.h"
class TimeoutHandler
{
	friend class TimerManager;
	friend class TimerWheel;
	friend class TimerHeap;
public:
	TimeoutHandler() {}

	virtual ~TimeoutHandler() {}

	virtual int HandleTimeout()
	{
		return 0;
	}

private:
	uint64	_expire;
	int		_timeout;
	int		_id;
};

#endif