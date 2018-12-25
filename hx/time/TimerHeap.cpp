#include "TimerHeap.h"
#include "Timestamp.h"

TimerHeap::TimerHeap():Super()
{
	_now = TimeStamp() * 1000 / StampPersecond();
}

TimerHeap::~TimerHeap(void)
{
}

int TimerHeap::Register(TimeoutHandler* handler,int ti)
{
	handler->_timeout = ti;
	handler->_expire = _now + ti;

	this->Push(handler);
	return 0;
}

int TimerHeap::CanPop(TimeoutHandler* obj)
{
	if (obj->_expire <= _now)
		return 1;
	return 0;
}

int TimerHeap::DoCompare(TimeoutHandler* left,TimeoutHandler* right)
{
	return left->_expire < right->_expire;
}

int TimerHeap::Update(uint64 now)
{
	_now = now * 1000 / StampPersecond();
	TimeoutHandler* handler = NULL;
	while (true)
	{
		this->Pop(handler);
		if (handler == NULL)
			break;
		if ( handler->HandleTimeout() == 0)
			this->Register(handler,handler->_timeout);
	}
	return 0;
}
