#include "Timer.h"

Timer::Timer() {
	callback_ = NULL;
	userdata_ = NULL;
}

Timer::~Timer() {
	Cancel();
}

int Timer::Start(Network::EventPoller* poller, float after, float repeat) {
	if (!callback_) {
		return -1;
	}

	timer_.set(poller->GetLoop());
	timer_.set(after, repeat);
	timer_.set<Timer, &Timer::OnTimeout>(this);
	timer_.start();

	return 0;
}

int Timer::Cancel() {
	if (timer_.is_active()) {
		timer_.stop();
		return 0;
	}
	return -1;
}

void Timer::OnTimeout(ev::timer &watcher, int revents) {
	callback_(this, userdata_);
}

bool Timer::IsActive() {
	return timer_.is_active();
}

void Timer::SetCallback(OnTimerout callback) {
	callback_ = callback;
}

void Timer::SetUserdata(void* userdata) {
	userdata_ = userdata;
}

void* Timer::GetUserdata() {
	return userdata_;
}
