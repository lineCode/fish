#ifndef TIMERENGINE_H
#define TIMERENGINE_H
#include <stdint.h>

template<class T>
class TimerEngineT {
public:
	TimerEngineT() {
		_engine = new T();
	}
	~TimerEngineT() {
		delete _engine;
	}

	int Register(TimeoutHandler* handler,int expire) {
		return _engine->Register(handler,expire);
	}

	int Update(uint64_t now) {
		return _engine->Update(now);
	}

private:
	T* _engine;
};

#endif