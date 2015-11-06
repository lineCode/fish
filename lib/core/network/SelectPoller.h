#ifndef SELECTPOLLER_H
#define SELECTPOLLER_H

#include "../platform.h"
#include "../Typedef.h"
#include "Interface.h"
#include "EventPoller.h"
#include <set>

namespace Network 
{
	class SelectPoller : public EventPoller
	{
	public:
		typedef std::set<int> FdSet;
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

		virtual void MakePiece(FdSet& set,std::vector<fd_set*>& result);

	protected:
		FishMap<int,int> _fdMap;
		FdSet _readSet;
		FdSet _writeSet;
		FdSet _errorSet;
	};

}

#endif