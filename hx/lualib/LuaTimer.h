#ifndef LUATIMER_H
#define LUATIMER_H

#include "time/Timer.h"
#include <iostream>
#include <functional>

class LuaTimer : public Timer {
	
public:
	typedef std::function<void(uint64_t)> OnTimerout;

public:
	LuaTimer();
	virtual ~LuaTimer();

	void SetCallback(OnTimerout callback);
	void SetTimerId(uint64_t timerId);

	virtual void HandleTimeout();

private:
	OnTimerout callback_;
	uint64_t timerId_;
};



#endif
