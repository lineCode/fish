#include "EventPoller.h"
#include "EpollPoller.h"
#include "SelectPoller.h"
#include "Network.h"

namespace Network
{
	EventPoller::EventPoller(int size)
	{
		_maxSize = size;

		for (int i = 1; i < _maxSize;i++)
			_idPool.push(i);
	
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

	bool EventPoller::HandleRead(int id) 
	{
		InputHandler* handler = _readHandles[id];

		assert(handler != NULL);
		handler->HandleInput();

		return true;
	}

	bool EventPoller::HandleWrite(int id) 
	{
		OutputHandler* handler = _writeHandles[id];

		assert(handler != NULL);
		handler->HandleOutput();

		return true;
	}

	bool EventPoller::HandleError(int id) 
	{
		ErrorHandler* handler = _errorHandles[id];
		assert(handler != NULL);

		handler->HandleError();

		return true;
	}

	int EventPoller::GenId(int fd)
	{
		if (_idPool.empty() == false)
		{
			int id = _idPool.front();
			_idPool.pop();
			return id;
		}
		return 0;
	}

	void EventPoller::RetrieveId(int fd,int id)
	{
		_idPool.push(id);
	}

	void EventPoller::AddError(int id)
	{
		this->_errorIds.push_back(id);
	}

	void EventPoller::ClearError()
	{
		this->_errorIds.clear();
	}

	void EventPoller::RegisterTimer(TimeoutHandler* handler,int ti)
	{
		_timerMgr.Register(handler,ti);
	}

	TimerManager& EventPoller::TimerMgr()
	{
		return _timerMgr;
	}
	
	EventPoller* EventPoller::Create() 
	{
#if defined( __WIN32__ ) || defined( WIN32 ) || defined( _WIN32 )
		return new SelectPoller();
#else
		return new EpollPoller();
#endif
	}
}
