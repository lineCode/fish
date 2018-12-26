#include "LuaTimer.h"

void LuaTimer::HandleTimeout() {
	if (callback_) {
		callback_(userdata_);
	} else {
		CancelTimer();
	}
}

void LuaTimer::SetCallback(OnTimerout callback) {
	callback_ = callback_;
}
	
void LuaTimer::SetUserdata(void* userdata) {
	userdata_ = userdata;
}
