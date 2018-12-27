#include "LuaTimer.h"

LuaTimer::LuaTimer() {
	callback_ = NULL;
	timerId_ = 0;
}

LuaTimer::~LuaTimer() {

}

void LuaTimer::SetCallback(OnTimerout callback) {
	callback_ = callback;
}
	
void LuaTimer::SetTimerId(uint64_t timerId) {
	timerId_ = timerId;
}

void LuaTimer::HandleTimeout() {
	if (callback_) {
		callback_(timerId_);
	} else {
		CancelTimer();
	}
}

