#include "LuaTimer.h"

void LuaTimer::HandleTimeout() {
	if (callback_) {
		callback_(this, userdata_);
	} else {
		CancelTimer();
	}
}

void LuaTimer::SetCallback(OnTimerout callback) {
	callback_ = callback;
}
	
void LuaTimer::SetUserdata(void* userdata) {
	userdata_ = userdata;
}
