#ifndef TIMER_H
#define TIMER_H
#include <functional>

#include "../Typedef.h"
#include "../network/EventPoller.h"

class Timer
{
public:
	Timer();

	virtual ~Timer();

	void StartTimer(Network::EventPoller* poller, float after, float repeat);

	void CancelTimer();

	bool IsActive();

	virtual void HandleTimeout() = 0;

private:
	void OnTimeout(ev::timer &watcher, int revents);

	ev::timer timer_;
};

#endif