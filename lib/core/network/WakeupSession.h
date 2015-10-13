#ifndef WAKEUPSESSION_H
#define WAKEUPSESSION_H

#include "Interface.h"

namespace Network
{
	class WakeupSession : public InputHandler
	{
	public:
		WakeupSession(EventPoller* poller,int wait);
		~WakeupSession(void);

		virtual void SetId(int id);

		virtual int GetId();

		virtual int Timerfd();

		virtual int HandleInput();

	protected:
		EventPoller* _poller;
		int _fd;
		int _id;
	};

}

#endif