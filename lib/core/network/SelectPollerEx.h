#ifndef SELECTPOLLEREX_H
#define SELECTPOLLEREX_H

#include "../platform.h"
#include "../Typedef.h"
#include "../util/ObjectPool.h"
#include "Interface.h"
#include "EventPoller.h"
#include "FdSet.h"

namespace Network 
{
	class SelectPollerEx : public EventPoller
	{
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

	protected:
		FishMap<int,int> _fdMap;
		FdSet _setRead;
		FdSet _setWrite;
		FdSet _setError;

		FdSet _tmpRead;
		FdSet _tmpWrite;
		FdSet _tmpError;
	};

}

#endif