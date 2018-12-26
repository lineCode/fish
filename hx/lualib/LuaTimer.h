#ifndef LUATIMER_H
#define LUATIMER_H

#include "time/Timer.h"
#include <iostream>
#include <functional>

class LuaTimer : public Timer {
public:
	typedef std::function<void(void*)> OnTimerout;
public:
	virtual void HandleTimeout();

	void SetCallback(OnTimerout callback);
	void SetUserdata(void* userdata);
private:
	OnTimerout callback_;
	void* userdata_;
}



#endif