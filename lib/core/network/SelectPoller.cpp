#include "SelectPoller.h"
#include "../time/Timestamp.h"
#include "../thread/Thread.h"
#include "../util/format.h"
#include "../Logger.h"


namespace Network 
{
	SelectPoller::SelectPoller(void):_readSet(),_writeSet(),_errorSet()
	{
#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
		WSAData wsdata;
		WSAStartup(0x202, &wsdata);
#endif
	}

	SelectPoller::~SelectPoller(void)
	{
	}

	bool SelectPoller::AddConnecter(int fd)
	{
		if (_errorSet.fd_count >= FD_SETSIZE)
		{
			LOG_ERROR(fmt::format("{}","SelectPoller::AddConnecter: error register fd max"));
			return false;
		}

		if (FD_ISSET(fd, &_errorSet))
			return false;

		FD_SET(fd, &_errorSet);

		return true;
	}

	bool SelectPoller::RemoveConnecter(int fd)
	{
		if (!FD_ISSET(fd, &_errorSet))
			return false;

		FD_CLR(fd, &_errorSet);

		return true;
	}

	bool SelectPoller::DoRegisterRead(int fd,int id) 
	{
		if (_readSet.fd_count >= FD_SETSIZE)
		{
			LOG_ERROR(fmt::format("{}","SelectPoller::DoRegisterRead: error register fd max"));
			return false;
		}

		if (FD_ISSET(fd,&_readSet))
			return false;

		FD_SET(fd,&_readSet);

		return true;
	}

	bool SelectPoller::DoRegisterWrite(int fd,int id) 
	{
		if (_readSet.fd_count >= FD_SETSIZE)
		{
			LOG_ERROR(fmt::format("{}","SelectPoller::DoRegisterWrite: error register fd max"));
			return false;
		}

		if (FD_ISSET(fd, &_writeSet))
			return false;

		FD_SET(fd, &_writeSet);

		++_writeCount;
		return true;
	}

	bool SelectPoller::DoDeRegisterRead(int fd,int id)
	{
		if (!FD_ISSET(fd, &_readSet))
			return false;

		FD_CLR(fd, &_readSet);

		return true;
	}

	bool SelectPoller::DoDeRegisterWrite(int fd,int id)
	{
		if (!FD_ISSET(fd, &_writeSet))
			return false;

		FD_CLR(fd, &_writeSet);

		return true;
	}

	int SelectPoller::GenId(int fd)
	{
		int id = EventPoller::GenId(fd);
		_fdMap[fd] = id;
		return id;
	}

	void SelectPoller::RetrieveId(int fd,int id)
	{
		_fdMap.erase(fd);
		EventPoller::RetrieveId(fd,id);
	}

	int SelectPoller::ProcessEvents() 
	{
		fd_set readsets;
		fd_set writesets;
		fd_set errorsets;

		FD_ZERO(&readsets);
		FD_ZERO(&writesets);
		FD_ZERO(&errorsets);

		readsets = _readSet;
		writesets = _writeSet;
		errorsets = _errorSet;

		struct timeval timeOut;
		timeOut.tv_sec = 0;
		timeOut.tv_usec = 10 * 1000;

		if (readsets.fd_count == 0 && writesets.fd_count == 0 && errorsets.fd_count == 0)
			Thread::Sleep(10);
		else
		{
			int nfds = select(0,&readsets,&writesets,&errorsets,&timeOut);

			if (nfds < 0)
			{
				LOG_ERROR(fmt::format("SelectPoller::select error:{}",WSAGetLastError()));
				return -1;
			}
			for (unsigned i=0; i < readsets.fd_count; ++i)
			{
				int fd = readsets.fd_array[i];
				--nfds;
				this->HandleRead(_fdMap[fd]);
			}

			for (unsigned i=0; i < writesets.fd_count; ++i)
			{
				int fd = writesets.fd_array[i];
				--nfds;
				this->HandleWrite(_fdMap[fd]);
			}

			for (unsigned i=0; i < errorsets.fd_count; ++i)
			{
				//only aync connect reach here
				int fd = errorsets.fd_array[i];
				--nfds;
				this->HandleError(_fdMap[fd]);
			}
		}
		
		_timerMgr.Update(TimeStamp());
		return 0;
	}
}
