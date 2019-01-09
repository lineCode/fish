#include "TimerWheel.h"
#include "Timestamp.h"
#include <assert.h>
#include <stdlib.h>

#define MAX_TIMEOUT (1000*3600*24-1)

TimerWheelMgr::TimerWheelMgr() {
	lastTime_ = 0;
	for ( int i = eWheelSec; i < lastTime_; i++ )
		wheels_[i] = new TimerWheel((TimerWheelMgr::Wheel)i,this);
}


TimerWheelMgr::~TimerWheelMgr(void) {
	for (uint32_t i = eWheelSec;i < eWheelEnd;i++)
		delete wheels_[i];
}

TimerWheel* TimerWheelMgr::GetWheel(Wheel what) {
	return wheels_[what];
}

int TimerWheelMgr::WheelSize(Wheel what) {
	switch(what) {
	case eWheelSec:
		return 1000;
	case eWheelHour:
		return 3600;
	case eWheelDay:
		return 24;
	default:
		break;
	}
	return 0;
}

int TimerWheelMgr::Precision(Wheel what) {
	switch(what) {
	case eWheelSec:
		return 1;
	case eWheelHour:
		return 1000;
	case eWheelDay:
		return 3600;
	default:
		break;
	}
	return 0;
}

int TimerWheelMgr::Register(TimeoutHandler* handler,int ti) {
	if ( ti == 0 || handler == NULL || ti > MAX_TIMEOUT ) {
		return -1;
	}
	
	if ( lastTime_ == 0 ) {
		lastTime_ = TimeStamp() * 1000 / StampPersecond();
	}
		
	handler->timeout_ = ti;
	handler->expire_ = lastTime_ + ti;

	TimerWheel* wheel = wheels_[eWheelSec];
	int slot = TimerWheelMgr::WheelSize(eWheelSec) - wheel->GetTick();

	if (slot > ti)
		wheel->Register(handler,ti);
	else {
		ti -= slot;
		ti /= TimerWheelMgr::WheelSize(eWheelSec);
		
		wheel = wheels_[eWheelHour];
		slot = TimerWheelMgr::WheelSize(eWheelHour) - wheel->GetTick();
		if ((int)slot > ti)
			wheel->Register(handler,ti);
		else {
			ti -= slot;
			ti /= TimerWheelMgr::WheelSize(eWheelHour);
			wheel = wheels_[eWheelHour];
			wheel->Register(handler,ti);
		}
	}

	return 0;
}

int TimerWheelMgr::Update(uint64_t now)
{
	now = now * 1000 / StampPersecond();
	if ( lastTime_ == 0 )
		lastTime_ = now;
	
	while ( lastTime_ != now )
		Fire(++lastTime_);
	return 0;
}

int TimerWheelMgr::Fire(uint64_t now) {
	if ( wheels_[eWheelSec]->NextTick() == 0 ) {
		wheels_[eWheelHour]->ShiftUp(now);
		if ( wheels_[eWheelHour]->NextTick() == 0 ) {
			wheels_[eWheelDay]->ShiftUp(now);
			wheels_[eWheelDay]->NextTick();
		}
	}
	wheels_[eWheelSec]->Trigger(now);
	return 0;
}

TimerWheel::TimerWheel(TimerWheelMgr::Wheel what,TimerWheelMgr* timerMgr)
{
	timeMgr_ = timerMgr;
	tick_ = 0;
	what_ = what;
	slots_ = (TimeoutHandlers**)malloc(TimerWheelMgr::WheelSize(what) * sizeof( *slots_ ));
	for (int i = 0;i < TimerWheelMgr::WheelSize(what);i++)
		slots_[i] = new TimeoutHandlers();
}


TimerWheel::~TimerWheel(void) {
	for ( int i = 0; i < TimerWheelMgr::WheelSize(what_); i++ )
		delete slots_[i];
	free(slots_);
}

int TimerWheel::GetTick() {
	return tick_;
}

int TimerWheel::NextTick() {
	tick_ = ( tick_ + 1 ) % TimerWheelMgr::WheelSize(what_);
	return tick_;
}

int TimerWheel::ShiftUp(uint64_t now) {
	if ( what_ == TimerWheelMgr::eWheelSec ) {
		return -1;
	}

	std::list<TimeoutHandler*>* handles = slots_[tick_];
	std::list<TimeoutHandler*>::iterator iter = handles->begin();
	for (;iter != handles->end();) {
		TimeoutHandler* handler = *iter;
		TimerWheel* lastWheel = timeMgr_->GetWheel(( TimerWheelMgr::Wheel )( what_ - 1 ));
		int remain = (int)( handler->expire_ - now ) - TimerWheelMgr::WheelSize(( TimerWheelMgr::Wheel )( lastWheel->what_ - 1 ));
		remain /= TimerWheelMgr::Precision(( TimerWheelMgr::Wheel )lastWheel->what_);
		lastWheel->Register(handler,remain);
		handles->erase(iter++);
	}
	return 0;
}

int TimerWheel::Trigger(uint64_t now) {
	std::list<TimeoutHandler*>* handles = slots_[tick_];
	std::list<TimeoutHandler*>::iterator iter = handles->begin();
	for (;iter != handles->end();) {
		TimeoutHandler* handler = *iter;
	
		if ( handler->HandleTimeout() == 0)
			timeMgr_->Register(handler,handler->timeout_);

		handles->erase(iter++);
	}
	return 0;
}

int TimerWheel::Register(TimeoutHandler* handler,int ti) {
	uint16_t slot = (this->tick_ + ti) % (TimerWheelMgr::WheelSize(what_));
	slots_[slot]->push_back(handler);

	return 0;
}