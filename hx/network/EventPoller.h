#ifndef EVENTPOLLER_H
#define EVENTPOLLER_H

#include <map>
#include <vector>
#include <queue>
#include "ev++.h"


namespace Network 
{
	class EventPoller
	{
	public:
		EventPoller();
		~EventPoller(void);

		virtual void Process();

		ev::default_loop& GetEvLoop();
		
	protected:
		ev::default_loop loop_;
	};

}

#endif