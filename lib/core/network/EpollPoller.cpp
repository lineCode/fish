#include "EpollPoller.h"

#include "../Typedef.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

namespace Network 
{
	EpollPoller::EpollPoller():_wakeup(this,100)
	{
		_epfd = epoll_create(1024);

		_wakeup.SetId(this->GenId(_wakeup.Timerfd()));
		this->RegisterRead(_wakeup.GetId(),_wakeup.Timerfd(),&_wakeup);
	}

	EpollPoller::~EpollPoller(void)
	{
		if (_epfd != -1)
			close(_epfd);
	}

	bool EpollPoller::DoRegister(int fd,int id,bool isRead,bool isRegister)
	{
		assert(id != 0);

		struct epoll_event ev;
		memset(&ev, 0, sizeof(ev));
		ev.data.u32 = id;

		int op;
		
		if (this->isRegistered(id, !isRead))
		{
			op = EPOLL_CTL_MOD;
			ev.events = isRegister ? EPOLLIN|EPOLLOUT : isRead ? EPOLLOUT : EPOLLIN;
		}
		else
		{
			op = isRegister ? EPOLL_CTL_ADD : EPOLL_CTL_DEL;
			ev.events = isRead ? EPOLLIN : EPOLLOUT;
		}

		if (epoll_ctl(_epfd, op, fd, &ev) < 0)
			assert(false);
		return true;
	}

	bool EpollPoller::DoRegisterRead(int fd,int id) 
	{
		return this->DoRegister(fd,id,true,true);
	}

	bool EpollPoller::DoRegisterWrite(int fd,int id) 
	{
		return this->DoRegister(fd,id,false,true);
	}

	bool EpollPoller::DoDeRegisterRead(int fd,int id) 
	{
		return this->DoRegister(fd,id,true,false);
	}

	bool EpollPoller::DoDeRegisterWrite(int fd,int id) 
	{
		return this->DoRegister(fd,id,false,false);
	}
	
	int EpollPoller::ProcessEvents() 
	{
		const int MAX_EVENTS = 128;
		struct epoll_event events[ MAX_EVENTS ];
		
		this->ClearError();

		int nfds = epoll_wait(_epfd,events,MAX_EVENTS,-1);

		for (int i = 0; i < nfds; ++i)
		{
			uint32 id = events[i].data.u32;
			
			if (events[i].events & (EPOLLERR|EPOLLHUP))
			{
				//only connect failed reach here
				this->AddError(id);
			}
			else
			{
				if (events[i].events & EPOLLIN)
					this->HandleRead(id);

				if (events[i].events & EPOLLOUT)
					this->HandleWrite(id);
			}
			
		
		}

		for (int i = 0;i < (int)_errorIds.size();i++)
		{
			int id = _errorIds[i];
			this->HandleError(id);
		}

		return true;
	}
}
