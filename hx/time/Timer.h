#ifndef TIMER_H
#define TIMER_H
#include <functional>

#include "Typedef.h"
#include "network/EventPoller.h"

class Timer {
public:
	typedef std::function<void(Timer*, void*)> OnTimerout;
public:
	Timer();

	virtual ~Timer();

	int Start(Network::EventPoller* poller, float after, float repeat);

	int Cancel();

	bool IsActive();

	void SetCallback(OnTimerout callback);

	void SetUserdata(void* userdata);

	void* GetUserdata();

private:
	void OnTimeout(ev::timer &watcher, int revents);

	ev::timer timer_;
	OnTimerout callback_;
	void* userdata_;
};

#endif