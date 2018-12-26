#include "Timer.h"

Timer::Timer() {

}

Timer::~Timer() {
	CancelTimer();
}

void Timer::StartTimer(Network::EventPoller* poller, float after, float repeat) {
	timer_.set(poller->GetEvLoop());
	timer_.set(after, repeat);
	timer_.set<Timer, &Timer::OnTimeout>(this);
	timer_.start();
}

void Timer::CancelTimer() {
	if (timer_.is_active()) {
		timer_.stop();
	}
}

void Timer::OnTimeout(ev::timer &watcher, int revents) {
	HandleTimeout();
}

bool Timer::IsActive() {
	return timer_.is_active();
}