#include "SelectPollerEx.h"
#include "../time/Timestamp.h"
#include "../thread/Thread.h"
#include "../util/format.h"
#include "../Logger.h"


namespace Network 
{
	SelectPollerEx::SelectPollerEx(void):_setRead(),_setWrite(),_setError(),_tmpRead(),_tmpWrite(),_tmpError()
	{
#if defined( WIN32 )
		WSAData wsdata;
		WSAStartup(0x202, &wsdata);
#endif
	}

	SelectPollerEx::~SelectPollerEx(void)
	{
	}

	bool SelectPollerEx::AddConnecter(int fd)
	{
		if (_setError.IsSet(fd))
			return false;
		
		_setError.Set(fd);
		return true;
	}

	bool SelectPollerEx::RemoveConnecter(int fd)
	{
		if (!_setError.IsSet(fd))
			return false;

		_setError.Clear(fd);
		return true;
	}

	bool SelectPollerEx::DoRegisterRead(int fd,int id) 
	{
		if (_setRead.IsSet(fd))
			return false;

		_setRead.Set(fd);
		return true;
	}

	bool SelectPollerEx::DoRegisterWrite(int fd,int id) 
	{
		if (_setWrite.IsSet(fd))
			return false;

		_setWrite.Set(fd);
		return true;
	}

	bool SelectPollerEx::DoDeRegisterRead(int fd,int id)
	{
		if (!_setRead.IsSet(fd))
			return false;

		_setRead.Clear(fd);
		return true;
	}

	bool SelectPollerEx::DoDeRegisterWrite(int fd,int id)
	{
		if (!_setWrite.IsSet(fd))
			return false;

		_setWrite.Clear(fd);
		return true;
	}

	int SelectPollerEx::GenId(int fd)
	{
		int id = EventPoller::GenId(fd);
		_fdMap[fd] = id;
		return id;
	}

	void SelectPollerEx::RetrieveId(int fd,int id)
	{
		_fdMap.erase(fd);
		EventPoller::RetrieveId(fd,id);
	}

	int SelectPollerEx::ProcessEvents() 
	{
		static int ti = 10;

		if (_setRead.Empty() && _setWrite.Empty() && _setError.Empty())
			Thread::Sleep(ti);
		else
		{
			_tmpRead.Copy(_setRead);
			_tmpWrite.Copy(_setWrite);
			_tmpError.Copy(_setError);

			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = ti;
			int nfds = select(0,(fd_set*)_tmpRead,(fd_set*)_tmpWrite,(fd_set*)_tmpError,&tv);

			if (nfds < 0)
			{
				LOG_ERROR(fmt::format("SelectPollerEx::select error:{}",WSAGetLastError()));
				return -1;
			}

			fd_set* set = ((fd_set*)_tmpRead);
			for (unsigned i=0; i < set->fd_count; ++i)
			{
				int fd = set->fd_array[i];
				--nfds;
				this->HandleRead(_fdMap[fd],fd);
			}
			
			set = ((fd_set*)_tmpWrite);
			for (unsigned i=0; i < set->fd_count; ++i)
			{
				int fd = set->fd_array[i];
				--nfds;
				this->HandleWrite(_fdMap[fd],fd);
			}
			
			set = ((fd_set*)_tmpError);
			for (unsigned i=0; i < set->fd_count; ++i)
			{
				//only aync connect reach here
				int fd = set->fd_array[i];
				--nfds;
				this->HandleError(_fdMap[fd],fd);
			}
		}

		_timerMgr.Update(TimeStamp());
		return 0;
	}
}
