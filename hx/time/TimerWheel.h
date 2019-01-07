#ifndef TIMERWHEEL_H
#define TIMERWHEEL_H
#include <list>
#include "TimerHandler.h"
#include "Typedef.h"

class TimerWheel;

class TimerWheelMgr {
public:
	enum Wheel {
		eWheelSec = 0,
		eWheelHour,
		eWheelDay,
		eWheelEnd
	};
public:
	TimerWheelMgr();

	~TimerWheelMgr();

	TimerWheel* GetWheel(Wheel what);

	static int WheelSize(Wheel what);

	static int Precision(Wheel what);

	int Register(TimeoutHandler* handler,int ti);

	int Update(uint64_t now);

private:
	int Fire(uint64_t now);

private:
	uint64_t lastTime_;
	TimerWheel*	wheels_[eWheelEnd];
};

class TimerWheel { 
public:
	typedef std::list<TimeoutHandler*> TimeoutHandlers;

public:
	TimerWheel(TimerWheelMgr::Wheel what,TimerWheelMgr* timerMgr);

	~TimerWheel(void);

	int GetTick();

	int NextTick();

	int ShiftUp(uint64_t now);

	int Trigger(uint64_t now);

	int Register(TimeoutHandler* handler,int ti);

protected:
	TimerWheelMgr* timeMgr_;
	TimerWheelMgr::Wheel what_;
	int tick_;
	TimeoutHandlers** slots_;
};
#endif