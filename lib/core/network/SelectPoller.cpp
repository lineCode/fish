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
		fd_set readFDs;
		fd_set writeFDs;
		fd_set errorFds;

		struct timeval timeOut;
		timeOut.tv_sec = 0;
		timeOut.tv_usec = 10 * 1000;

		FD_ZERO(&readFDs);
		FD_ZERO(&writeFDs);
		FD_ZERO(&errorFds);

		readFDs = _readSet;
		writeFDs = _writeSet;
		errorFds = _errorSet;

		this->ClearError();

		if (readFDs.fd_count == 0 && writeFDs.fd_count == 0)
			Thread::Sleep(10);
		else
		{
			int countReady = select(0,&readFDs,&writeFDs,&errorFds,&timeOut);

			if (countReady < 0)
			{
				LOG_ERROR(fmt::format("SelectPoller::select error:{}",WSAGetLastError()));
				return -1;
			}
			for (unsigned i=0; i < readFDs.fd_count; ++i)
			{
				int fd = readFDs.fd_array[ i ];
				--countReady;
				this->HandleRead(_fdMap[fd]);
			}

			for (unsigned i=0; i < writeFDs.fd_count; ++i)
			{
				int fd = writeFDs.fd_array[ i ];
				--countReady;
				this->HandleWrite(_fdMap[fd]);
			}

			for (unsigned i=0; i < errorFds.fd_count; ++i)
			{
				//only aync connect reach here
				int fd = errorFds.fd_array[ i ];
				--countReady;
				this->HandleError(_fdMap[fd]);
			}

			for (int i = 0;i < (int)_errorIds.size();i++)
			{
				int id = _errorIds[i];
				this->HandleError(id);
			}
		}
		
		_timerMgr.Update(TimeStamp());
		return 0;
	}
}
