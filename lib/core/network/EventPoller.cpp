#include "EventPoller.h"
#include "EpollPoller.h"
#include "Network.h"

#include "ev++.h"
#if defined(WIN32)
#include "SelectPoller.h"
#endif

namespace Network
{
	EventPoller::EventPoller() :loop_()
	{

	}

	EventPoller::~EventPoller(void)
	{
	}

	int EventPoller::Process()
	{
		return loop_.run();
	}

	ev::default_loop& EventPoller::GetEvLoop()
	{
		return loop_;
	}
}
