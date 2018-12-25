#include "EventPoller.h"
#include "Network.h"

#include "ev++.h"

namespace Network
{
	EventPoller::EventPoller() :loop_()
	{

	}

	EventPoller::~EventPoller(void)
	{
	}

	void EventPoller::Process()
	{
		return loop_.run();
	}

	ev::default_loop& EventPoller::GetEvLoop()
	{
		return loop_;
	}
}
