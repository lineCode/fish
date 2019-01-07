#include "EventPoller.h"
#include "Network.h"

#include "ev++.h"

namespace Network {
	EventPoller::EventPoller() :loop_() {
	}

	EventPoller::~EventPoller(void) {
	}

	void EventPoller::Process() {
		loop_.run();
	}

	void EventPoller::Break() {
		loop_.break_loop();
	}

	ev::default_loop& EventPoller::GetLoop() {
		return loop_;
	}
}
