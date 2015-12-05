#include "EventPoller.h"
#include "EpollPoller.h"
#include "Network.h"

#if defined(WIN32)
#include "SelectPoller.h"
#endif

namespace Network
{
	EventPoller::EventPoller(int size)
	{
		_maxSize = size;

		for (int i = 1; i < _maxSize;i++)
			_idPool.push(i);
	
		_idMap.resize(_maxSize);
		_readHandles.resize(_maxSize);
		_writeHandles.resize(_maxSize);
		_errorHandles.resize(_maxSize);
	}

	EventPoller::~EventPoller(void)
	{
	}

	bool EventPoller::AddConnecter(int fd)
	{
		return true;
	}

	bool EventPoller::RemoveConnecter(int fd)
	{
		return true;
	}

	bool EventPoller::RegisterRead(int id,int fd,InputHandler* handler)
	{
		if (!this->DoRegisterRead(fd,id))
			return false;

		assert(_readHandles[id] == NULL);
		_readHandles[id] = handler;

		return true;
	}

	bool EventPoller::RegisterWrite(int id,int fd,OutputHandler* handler) 
	{
		if (!this->DoRegisterWrite(fd,id))
			return false;

		assert(_writeHandles[id] == NULL);
		_writeHandles[id] = handler;

		return true;
	}

	bool EventPoller::RegisterError(int id,ErrorHandler* handler) 
	{
		assert(_errorHandles[id] == NULL);
		_errorHandles[id] = handler;
		return true;
	}

	bool EventPoller::DeRegisterRead(int id,int fd) 
	{
		assert(_readHandles[id] != NULL);
		_readHandles[id] = NULL;
		return DoDeRegisterRead(fd,id);
	}

	bool EventPoller::DeRegisterWrite(int id,int fd) 
	{
		assert(_writeHandles[id] != NULL);
		_writeHandles[id] = NULL;
		return DoDeRegisterWrite(fd,id);
	}

	bool EventPoller::DeRegisterError(int id)
	{
		assert(_errorHandles[id] != NULL);
		_errorHandles[id] = NULL;
		return true;
	}

	bool EventPoller::isRegistered(int id, bool isForRead)
	{
		return isForRead ? (_readHandles[id] != NULL) : (_writeHandles[id] != NULL);
	}

	bool EventPoller::isRegisteredError(int id)
	{
		return _errorHandles[id] != NULL;
	}

	int EventPoller::Process()
	{
		return this->ProcessEvents();
	}

	bool EventPoller::HandleRead(int id,int fd) 
	{
		int ofd = _idMap[id];
		if (ofd != fd)
		{
			//避免id回绕
			fprintf(stderr,"HandleRead error,id:%d not math[%d:%d]\n",id,fd,ofd);
			return false;
		}

		InputHandler* handler = _readHandles[id];

		if (handler == NULL)
		{
			//避免在回调此id前对应的handler已经销毁
			fprintf(stderr,"HandleRead error,id:%d not found\n",id);
			return false;
		}

		handler->HandleInput();
		return true;
	}

	bool EventPoller::HandleWrite(int id,int fd) 
	{
		int ofd = _idMap[id];
		if (ofd != fd)
		{
			//避免id回绕
			fprintf(stderr,"HandleWrite error,id:%d not math[%d:%d]\n",id,fd,ofd);
			return false;
		}

		OutputHandler* handler = _writeHandles[id];

		if (handler == NULL)
		{
			//避免在回调此id前对应的handler已经销毁
			fprintf(stderr,"HandleWrite error,id:%d not found\n",id);
			return false;
		}

		handler->HandleOutput();
		return true;
	}

	bool EventPoller::HandleError(int id,int fd) 
	{
		int ofd = _idMap[id];
		if (ofd != fd)
		{
			//避免id回绕
			fprintf(stderr,"HandleError error,id:%d not math[%d:%d]\n",id,fd,ofd);
			return false;
		}

		ErrorHandler* handler = _errorHandles[id];

		if (handler == NULL)
		{
			//避免在回调此id前对应的handler已经销毁
			fprintf(stderr,"HandleError error,id:%d not found\n",id);
			return false;
		}

		handler->HandleError();
		return true;
	}

	int EventPoller::GenId(int fd)
	{
		if (_idPool.empty() == false)
		{
			int id = _idPool.front();
			_idPool.pop();
			_idMap[id] = fd;
			return id;
		}
		return 0;
	}

	void EventPoller::RetrieveId(int fd,int id)
	{
		_idMap[id] = 0;
		_idPool.push(id);
	}

	TimerEngine& EventPoller::Timer()
	{
		return _timerEngine;
	}

	EventPoller* EventPoller::Create() 
	{
#if defined( WIN32 )
		return new SelectPoller();
#else
		return new EpollPoller();
#endif
	}
}
