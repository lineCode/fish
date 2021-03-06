﻿#ifndef EVENTPOLLER_H
#define EVENTPOLLER_H

#include <map>
#include <vector>
#include <queue>

#include "ev++.h"


namespace Network {
	class EventPoller {
	public:
		EventPoller();
		virtual ~EventPoller(void);

		virtual void Process();

		virtual void Break();

		ev::default_loop& GetLoop();
		
	protected:
		ev::default_loop loop_;
	};

}

#endif
