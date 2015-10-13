#ifndef TIMERMANAGER_H
#define TIMERMANAGER_H
#include <list>
#include "TimerHandler.h"
#include "../Typedef.h"

class TimerWheel;

class TimerManager
{
public:
	enum Wheel
	{
		WheelSec = 0,
		WheelHour,
		WheelDay,
		WheelEnd
	};
public:
	TimerManager();

	~TimerManager();

	TimerWheel* GetWheel(Wheel what);

	static int WheelSize(Wheel what);

	static int Precision(Wheel what);

	int Register(TimeoutHandler* handler,int ti);

	int Update(uint64 now);

private:
	int Fire(uint64 now);

private:
	uint64		_lastTime;
	TimerWheel*	_wheels[WheelEnd];
};

class TimerWheel
{ 
public:
	typedef std::list<TimeoutHandler*> TimeoutHandlers;

public:
	TimerWheel(TimerManager::Wheel what,TimerManager* timerMgr);

	~TimerWheel(void);

	int GetTick();

	int NextTick();

	int ShiftUp(uint64 now);

	int Trigger(uint64 now);

	int Register(TimeoutHandler* handler,int ti);

protected:
	TimerManager*		_timerMgr;
	TimerManager::Wheel _what;
	int					_tick;
	TimeoutHandlers**	_slots;
};
#endif