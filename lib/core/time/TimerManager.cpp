#include "TimerManager.h"
#include "Timestamp.h"
#include <assert.h>
#include <stdlib.h>

#define MAX_TIMEOUT (1000*3600*24-1)

TimerManager::TimerManager()
{
	_lastTime = 0;
	for (int i = WheelSec;i < WheelEnd;i++)
		_wheels[i] = new TimerWheel((TimerManager::Wheel)i,this);
}


TimerManager::~TimerManager(void)
{
	for (int i = WheelSec;i < WheelEnd;i++)
		delete _wheels[i];
}

TimerWheel* TimerManager::GetWheel(Wheel what)
{
	return _wheels[what];
}

int TimerManager::WheelSize(Wheel what)
{
	switch(what)
	{
	case WheelSec:
		return 1000;
	case WheelHour:
		return 3600;
	case WheelDay:
		return 24;
	default:
		break;
	}
	return 0;
}

int TimerManager::Precision(Wheel what)
{
	switch(what)
	{
	case WheelSec:
		return 1;
	case WheelHour:
		return 1000;
	case WheelDay:
		return 3600;
	default:
		break;
	}
	return 0;
}

int TimerManager::Register(TimeoutHandler* handler,int ti)
{
	if (ti == 0 || handler == NULL || ti > MAX_TIMEOUT)
		return -1;

	if (_lastTime == 0)
		_lastTime = TimeStamp() * 1000 / StampPersecond();

	handler->_timeout = ti;
	handler->_expire = _lastTime + ti;

	TimerWheel* wheel = _wheels[WheelSec];
	int slot = TimerManager::WheelSize(WheelSec) - wheel->GetTick();

	if (slot > ti)
		wheel->Register(handler,ti);
	else
	{
		ti -= slot;
		ti /= TimerManager::WheelSize(WheelSec);
		
		wheel = _wheels[WheelHour];
		slot = TimerManager::WheelSize(WheelHour) - wheel->GetTick();
		if ((int)slot > ti)
			wheel->Register(handler,ti);
		else
		{
			ti -= slot;
			ti /= TimerManager::WheelSize(WheelHour);
			wheel = _wheels[WheelHour];
			wheel->Register(handler,ti);
		}
	}

	return 0;
}

int TimerManager::Update(uint64 now)
{
	now = now * 1000 / StampPersecond();
	if (_lastTime == 0)
		_lastTime = now;
	
	while (_lastTime != now)
		Fire(++_lastTime);
	return 0;
}

int TimerManager::Fire(uint64 now)
{
	if (_wheels[WheelSec]->NextTick() == 0)
	{
		_wheels[WheelHour]->ShiftUp(now);
		if (_wheels[WheelHour]->NextTick() == 0)
		{
			_wheels[WheelDay]->ShiftUp(now);
			_wheels[WheelDay]->NextTick();
		}
	}
	_wheels[WheelSec]->Trigger(now);
	return 0;
}

TimerWheel::TimerWheel(TimerManager::Wheel what,TimerManager* timerMgr)
{
	_timerMgr = timerMgr;
	_tick = 0;
	_what = what;
	_slots = (TimeoutHandlers**)malloc(TimerManager::WheelSize(what) * sizeof(*_slots));
	for (int i = 0;i < TimerManager::WheelSize(what);i++)
		_slots[i] = new TimeoutHandlers();
}


TimerWheel::~TimerWheel(void)
{
	for (int i = 0;i < TimerManager::WheelSize(_what);i++)
		delete _slots[i];
	free(_slots);
}

int TimerWheel::GetTick()
{
	return _tick;
}

int TimerWheel::NextTick()
{
	_tick = (_tick+1) % TimerManager::WheelSize(_what);
	return _tick;
}

int TimerWheel::ShiftUp(uint64 now)
{
	if (_what == TimerManager::WheelSec)
		return -1;

	std::list<TimeoutHandler*>* handles = _slots[_tick];
	std::list<TimeoutHandler*>::iterator iter = handles->begin();
	for (;iter != handles->end();)
	{
		TimeoutHandler* handler = *iter;
		TimerWheel* lastWheel = _timerMgr->GetWheel((TimerManager::Wheel)(_what-1));
		int remain = (int)(handler->_expire - now) - TimerManager::WheelSize((TimerManager::Wheel)(lastWheel->_what-1));
		remain /= TimerManager::Precision((TimerManager::Wheel)lastWheel->_what);
		lastWheel->Register(handler,remain);
		handles->erase(iter++);
	}
	return 0;
}

int TimerWheel::Trigger(uint64 now)
{
	std::list<TimeoutHandler*>* handles = _slots[_tick];
	std::list<TimeoutHandler*>::iterator iter = handles->begin();
	for (;iter != handles->end();)
	{
		TimeoutHandler* handler = *iter;
	
		if ( handler->HandleTimeout() == 0)
			_timerMgr->Register(handler,handler->_timeout);

		handles->erase(iter++);
	}
	return 0;
}

int TimerWheel::Register(TimeoutHandler* handler,int ti)
{
	uint16 slot = (this->_tick + ti) % (TimerManager::WheelSize(_what));
	_slots[slot]->push_back(handler);

	return 0;
}