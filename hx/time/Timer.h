#ifndef TIMER_H
#define TIMER_H
#include <functional>

#include "../Typedef.h"
#include "../network/EventPoller.h"

class Timer
{
public:
	Timer() {}

	virtual ~Timer() {}

	void StartTimer(Network::EventPoller* poller, float after, float repeat)
	{
		timer_.set(poller->GetEvLoop());
		timer_.set(after, repeat);
		timer_.set<Timer, &Timer::OnTimeout>(this);
		timer_.start();
	}

	virtual void HandleTimeout() = 0;

private:
	void OnTimeout(ev::timer &watcher, int revents)
	{
		HandleTimeout();
	}

	ev::timer timer_;
};

#endif