#ifndef EPOLLPOLLER_H
#define EPOLLPOLLER_H

#include "EventPoller.h"
#include "Interface.h"
#include "WakeupSession.h"
namespace Network 
{
	class EpollPoller : public EventPoller
	{
	public:
		EpollPoller();
		~EpollPoller();

		bool DoRegister(int fd,int id,bool isRead,bool isRegister);

		virtual bool DoRegisterRead(int fd,int id);
		virtual bool DoRegisterWrite(int fd,int id);

		virtual bool DoDeRegisterRead(int fd,int id);
		virtual bool DoDeRegisterWrite(int fd,int id);

		virtual int ProcessEvents();

	protected:
		int _epfd;
		WakeupSession _wakeup;
	};

}

#endif