#ifndef THEAD_H
#define THEAD_H

#if defined(WIN32)
#include <winsock2.h>
#include <windows.h> 
#endif

#include "../Typedef.h"

namespace Thread 
{
	class WorkerThread
	{
	public:
		WorkerThread();
		~WorkerThread();

		void Start();
		bool Join();

		virtual void Run() = 0;

		virtual void Wait();
		virtual void Wakeup();

		virtual void initCond(void);

		virtual void initMutex(void);

		virtual void deleteCond(void);

		virtual void deleteMutex(void);

		virtual void lock(void);

		virtual void unlock(void);	

#if defined( WIN32 )
		static unsigned __stdcall threadFunc(void *ctx);
#else
		static void*			  threadFunc(void* ctx);
#endif

	protected:
		THREAD_ID		_threadId;
		THREAD_SINGNAL  _cond;		
		THREAD_MUTEX	_mutex;		
	};


	void Sleep(int wait);
}

#endif