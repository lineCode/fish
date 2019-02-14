#include "TimerHeap.h"
#include "Timestamp.h"

TimerHeap::TimerHeap():Super() {
	now_ = (uint64_t)GetTimeMillis();
}

TimerHeap::~TimerHeap(void) {
}

int TimerHeap::Register(TimeoutHandler* handler,int ti) {
	handler->timeout_ = ti;
	handler->expire_ = now_ + ti;
	this->Push(handler);
	return 0;
}

int TimerHeap::CanPop(TimeoutHandler* obj) {
	if ( obj->expire_ <= now_ )
		return 1;
	return 0;
}

int TimerHeap::DoCompare(TimeoutHandler* left,TimeoutHandler* right) {
	return left->expire_ < right->expire_;
}

int TimerHeap::Update(uint64_t now) {
	now_ = now;
	TimeoutHandler* handler = NULL;
	while (true) {
		this->Pop(handler);
		if (handler == NULL)
			break;
		if ( handler->HandleTimeout() == 0)
			this->Register(handler,handler->timeout_);
	}
	return 0;
}
