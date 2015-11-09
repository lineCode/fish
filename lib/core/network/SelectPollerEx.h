#ifndef SELECTPOLLEREX_H
#define SELECTPOLLEREX_H

#include "../Platform.h"
#include "../Typedef.h"
#include "../util/ObjectPool.h"
#include "Interface.h"
#include "EventPoller.h"

#include <set>

namespace Network 
{
	class SelectPollerEx : public EventPoller
	{
	public:
		typedef std::set<int> FdSet;
	public:
		SelectPollerEx();
		~SelectPollerEx();

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
		ObjectPool<fd_set> _fdsetPool;
		FdSet _readSet;
		FdSet _writeSet;
		FdSet _errorSet;
	};

}

#endif