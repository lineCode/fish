#ifndef TIMER_H
#define TIMER_H
#include <functional>

#include "../Typedef.h"
#include "../network/EventPoller.h"

class Timer
{
public:
	typedef std::function<void(void*)> OnTimeout;

public:
	Timer(OnTimeout callback, void* ud) :timeoutCallback_(callback), ud_(ud)
	{
	}

	virtual ~Timer() {}

	void RegsiterTimer(Network::EventPoller* poller, int after, int repeat, void* ud)
	{
		ud_ = ud;

		io_.set(poller->GetEvLoop());
		io_.set(after, repeat);
		io_.set<Timer, &Timer::HandleTimeout>(this);
		io_.start();
	}

public:
	void HandleTimeout(ev::timer &watcher, int revents)
	{
		timeoutCallback_(ud_);
	}

private:
	void* ud_;
	ev::timer io_;
	OnTimeout timeoutCallback_;
};

#endif