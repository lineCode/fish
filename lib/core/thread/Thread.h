#ifndef THEAD_H
#define THEAD_H


#if defined( WIN32 )
#include "../platform.h"
#include <process.h>  

#define THREAD_ID											HANDLE
#define THREAD_SINGNAL										HANDLE
#define THREAD_SINGNAL_INIT(x)								x = CreateEvent(NULL, TRUE, FALSE, NULL)
#define THREAD_SINGNAL_DELETE(x)							CloseHandle(x)
#define THREAD_SINGNAL_SET(x)								SetEvent(x)
#define THREAD_MUTEX										CRITICAL_SECTION
#define THREAD_MUTEX_INIT(x)								InitializeCriticalSection(&x)
#define THREAD_MUTEX_DELETE(x)								DeleteCriticalSection(&x)
#define THREAD_MUTEX_LOCK(x)								EnterCriticalSection(&x)
#define THREAD_MUTEX_UNLOCK(x)								LeaveCriticalSection(&x)
#else

#include <pthread.h>
#include <unistd.h>
#define THREAD_ID											pthread_t
#define THREAD_SINGNAL										pthread_cond_t
#define THREAD_SINGNAL_INIT(x)								pthread_cond_init(&x, NULL)
#define THREAD_SINGNAL_DELETE(x)							pthread_cond_destroy(&x)
#define THREAD_SINGNAL_SET(x)								pthread_cond_signal(&x);
#define THREAD_MUTEX										pthread_mutex_t
#define THREAD_MUTEX_INIT(x)								pthread_mutex_init (&x, NULL)
#define THREAD_MUTEX_DELETE(x)								pthread_mutex_destroy(&x)
#define THREAD_MUTEX_LOCK(x)								pthread_mutex_lock(&x)
#define THREAD_MUTEX_UNLOCK(x)								pthread_mutex_unlock(&x)		

#endif

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