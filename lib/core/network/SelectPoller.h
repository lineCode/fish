#ifndef SELECTPOLLER_H
#define SELECTPOLLER_H

#if defined(WIN32)
#include <WinSock2.h>
#endif

#include "Interface.h"
#include "EventPoller.h"
#include "../Typedef.h"

namespace Network 
{
	class SelectPoller : public EventPoller
	{
	public:
		SelectPoller();
		~SelectPoller();

		virtual bool AddConnecter(int fd);
		virtual bool RemoveConnecter(int fd);

		virtual bool DoRegisterRead(int fd,int id);
		virtual bool DoRegisterWrite(int fd,int id);

		virtual bool DoDeRegisterRead(int fd,int id);
		virtual bool DoDeRegisterWrite(int fd,int id);

		virtual int GenId(int fd);
		virtual void RetrieveId(int fd,int id);

		virtual int ProcessEvents();

	protected:
		FishMap<int,int> _fdMap;
		fd_set _readSet;
		fd_set _writeSet;
		fd_set _errorSet;
	};

}

#endif