#include "SelectPollerEx.h"
#include "../time/Timestamp.h"
#include "../thread/Thread.h"
#include "../util/format.h"
#include "../Logger.h"


namespace Network 
{
	SelectPollerEx::SelectPollerEx(void):_fdsetPool("fd_set"),_readSet(),_writeSet(),_errorSet()
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
		if (_errorSet.find(fd) != _errorSet.end())
			return false;

		_errorSet.insert(fd);
		return true;
	}

	bool SelectPollerEx::RemoveConnecter(int fd)
	{
		if (_errorSet.find(fd) == _errorSet.end())
			return false;

		_errorSet.erase(fd);
		return true;
	}

	bool SelectPollerEx::DoRegisterRead(int fd,int id) 
	{
		if (_readSet.find(fd) != _readSet.end())
			return false;

		_readSet.insert(fd);
		return true;
	}

	bool SelectPollerEx::DoRegisterWrite(int fd,int id) 
	{
		if (_writeSet.find(fd) != _writeSet.end())
			return false;

		_writeSet.insert(fd);
		return true;
	}

	bool SelectPollerEx::DoDeRegisterRead(int fd,int id)
	{
		if (_readSet.find(fd) == _readSet.end())
			return false;

		_readSet.erase(fd);
		return true;
	}

	bool SelectPollerEx::DoDeRegisterWrite(int fd,int id)
	{
		if (_writeSet.find(fd) == _writeSet.end())
			return false;

		_writeSet.erase(fd);
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

		if (_readSet.size() == 0 && _writeSet.size() == 0 && _errorSet.size() == 0)
			Thread::Sleep(ti);
		else
		{
			std::vector<fd_set*> readSets;
			std::vector<fd_set*> writeSets;
			std::vector<fd_set*> errorSets;

			MakePiece(_readSet,readSets);
			MakePiece(_writeSet,writeSets);
			MakePiece(_errorSet,errorSets);

			int readIndex = 0;
			int readCount = readSets.size();

			int writeIndex = 0;
			int writeCount = writeSets.size();

			int errorIndex = 0;
			int errorCount = errorSets.size();

			for (;;)
			{
				fd_set* readset = readIndex < readCount ? readSets[readIndex++]:NULL;
				fd_set* writeset = writeIndex < writeCount ? writeSets[writeIndex++]:NULL;
				fd_set* errorset = errorIndex < errorCount ? errorSets[errorIndex++]:NULL;

				if (readset == NULL && writeset == NULL && errorset == NULL)
				{
					Thread::Sleep(ti);
					break;
				}
				else
				{
					struct timeval tv;
					tv.tv_sec = 0;
					tv.tv_usec = 0;
					int nfds = select(0,readset,writeset,errorset,&tv);

					if (nfds < 0)
					{
						LOG_ERROR(fmt::format("SelectPollerEx::select error:{}",WSAGetLastError()));
						return -1;
					}

					if (readset != NULL)
					{
						for (unsigned i=0; i < readset->fd_count; ++i)
						{
							int fd = readset->fd_array[i];
							--nfds;
							this->HandleRead(_fdMap[fd],fd);
						}
						_fdsetPool.Push(readset);
					}
					
					if (writeset != NULL)
					{
						for (unsigned i=0; i < writeset->fd_count; ++i)
						{
							int fd = writeset->fd_array[i];
							--nfds;
							this->HandleWrite(_fdMap[fd],fd);
						}
						_fdsetPool.Push(writeset);
					}
					
					if (errorset != NULL)
					{
						for (unsigned i=0; i < errorset->fd_count; ++i)
						{
							//only aync connect reach here
							int fd = errorset->fd_array[i];
							--nfds;
							this->HandleError(_fdMap[fd],fd);
						}
						_fdsetPool.Push(errorset);
					}
				}
			}
		}
		
		_timerMgr.Update(TimeStamp());
		return 0;
	}

	void SelectPollerEx::MakePiece(FdSet& set,std::vector<fd_set*>& result)
	{
		fd_set* tmpSets;
		_fdsetPool.Pop(tmpSets);
		assert(tmpSets != NULL);
		result.push_back(tmpSets);
		FD_ZERO(tmpSets);

		for (FdSet::iterator iter = set.begin();iter != set.end();iter++)
		{
			if (tmpSets->fd_count >= FD_SETSIZE)
			{
				tmpSets = NULL;
				_fdsetPool.Pop(tmpSets);
				assert(tmpSets != NULL);
				result.push_back(tmpSets);
				FD_ZERO(tmpSets);
			}
			FD_SET(*iter,tmpSets);
		}
	}
}
