#ifndef LUATIMER_H
#define LUATIMER_H

#include "time/Timer.h"
#include <iostream>
#include <functional>

class LuaTimer : public Timer {
	
public:
	typedef std::function<void(LuaTimer*, void*)> OnTimerout;

public:
	LuaTimer();
	virtual ~LuaTimer();

	void SetCallback(OnTimerout callback);
	void SetUserdata(void* userdata);

	virtual void HandleTimeout();

private:
	OnTimerout callback_;
	void* userdata_;
};



#endif
