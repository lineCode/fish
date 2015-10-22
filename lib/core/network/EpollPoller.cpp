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
		ev.data.u64 = id | (fd << 32);

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

		int nfds = epoll_wait(_epfd,events,MAX_EVENTS,-1);

		for (int i = 0; i < nfds; ++i)
		{
			uint64 mask = events[i].data.u64;
			
			if (events[i].events & (EPOLLERR|EPOLLHUP))
				this->HandleError(mask&0xffffffff,mask >> 32);
			else
			{
				if (events[i].events & EPOLLIN)
					this->HandleRead(mask&0xffffffff,mask >> 32);

				if (events[i].events & EPOLLOUT)
					this->HandleWrite(mask&0xffffffff,mask >> 32);
			}
		}

		return true;
	}
}
