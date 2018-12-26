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
	
void LuaTimer::SetUserdata(void* userdata) {
	userdata_ = userdata;
}

void LuaTimer::HandleTimeout() {
	if (callback_) {
		callback_(this, userdata_);
	} else {
		CancelTimer();
	}
}
