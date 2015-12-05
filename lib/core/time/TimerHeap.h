#ifndef TIMERHEAP_H
#define TIMERHEAP_H
#include "../util/MiniheapT.h"
#include "TimerHandler.h"

class TimerHeap : public MiniheapT<TimeoutHandler>
{
public:
	typedef MiniheapT<TimeoutHandler> Super;

public:
	TimerHeap(int size);
	~TimerHeap();

	virtual int Register(TimeoutHandler* handler,int expire);

	virtual int CanPop(TimeoutHandler* handler);

	virtual int DoCompare(TimeoutHandler* left,TimeoutHandler* right);
};

#endif