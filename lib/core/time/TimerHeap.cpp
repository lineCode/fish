#include "TimerHeap.h"


TimerHeap::TimerHeap(int size):Super(size)
{
}


TimerHeap::~TimerHeap(void)
{
}

int TimerHeap::Register(TimeoutHandler* handler,int expire)
{
	return 0;
}

int TimerHeap::CanPop(TimeoutHandler* obj)
{
	return 0;
}

int TimerHeap::DoCompare(TimeoutHandler* left,TimeoutHandler* right)
{
	return 0;
}
