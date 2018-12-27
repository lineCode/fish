#include "LuaTimer.h"

LuaTimer::LuaTimer() {
	callback_ = NULL;
	userdata_ = NULL;
}

LuaTimer::~LuaTimer() {

}

void LuaTimer::SetCallback(OnTimerout callback) {
	callback_ = callback;
}
	
uint64_t LuaTimer::SetTimerId(uint64_t timerId) {
	timerId_ = timerId;
}

void LuaTimer::HandleTimeout() {
	if (callback_) {
		callback_(timerId);
	} else {
		CancelTimer();
	}
}
