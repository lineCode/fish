
#include "Thread.h"

#ifndef MUTEX_H
#define MUTEX_H

namespace Thread {
	class Mutex
	{
	public:
		Mutex() {
			THREAD_MUTEX_INIT(_mutex);
		}
		~Mutex() {
			THREAD_MUTEX_DELETE(_mutex);
		}

		void lock() {
			THREAD_MUTEX_LOCK(_mutex);
		}

		void unlock() {
			THREAD_MUTEX_UNLOCK(_mutex);
		}

	protected:
		THREAD_MUTEX	_mutex;
	};

	class MutexGuard
	{
	public:
		MutexGuard(Mutex& mutex)
			: _mutex(mutex)
		{
			_mutex.lock();
		}

		~MutexGuard()
		{
			_mutex.unlock();
		}

	private:

		Mutex& _mutex;
	};



}

#endif