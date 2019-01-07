#ifndef TIMERHEAP_H
#define TIMERHEAP_H
#include <stdint.h>
#include "util/MiniheapT.h"
#include "TimerHandler.h"

class TimerHeap : public MiniheapT<TimeoutHandler> {
public:
	typedef MiniheapT<TimeoutHandler> Super;

public:
	TimerHeap();

	virtual ~TimerHeap();

	virtual int Register(TimeoutHandler* handler,int expire);

	virtual int CanPop(TimeoutHandler* handler);

	virtual int DoCompare(TimeoutHandler* left,TimeoutHandler* right);

	virtual int Update(uint64_t now);

private:
	uint64_t now_;
};

#endif