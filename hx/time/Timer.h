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
		io_.set(poller->GetEvLoop());
		io_.set(after, repeat);
		io_.set<Timer, &Timer::OnTimeout>(this);
		io_.start();
	}

	void OnTimeout(ev::timer &watcher, int revents)
	{
		HandleTimeout();
	}

	virtual void HandleTimeout() = 0;

private:
	ev::timer io_;
};

#endif