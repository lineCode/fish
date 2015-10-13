#include "Thread.h"
#include <stdio.h>

namespace Thread 
{


WorkerThread::WorkerThread() 
{
}

WorkerThread::~WorkerThread() 
{
}

void WorkerThread::initCond(void)
{
	THREAD_SINGNAL_INIT(_cond);
}

void WorkerThread::initMutex(void)
{
	THREAD_MUTEX_INIT(_mutex);	
}

void WorkerThread::deleteCond(void)
{
	THREAD_SINGNAL_DELETE(_cond);
}

void WorkerThread::deleteMutex(void)
{
	THREAD_MUTEX_DELETE(_mutex);
}

void WorkerThread::lock(void)
{
	THREAD_MUTEX_LOCK(_mutex); 
}

void WorkerThread::unlock(void)
{
	THREAD_MUTEX_UNLOCK(_mutex); 
}	
void WorkerThread::Start() 
{
#if defined( WIN32 )
	_threadId = (THREAD_ID)_beginthreadex(NULL, 0, &WorkerThread::threadFunc, (void*)this, NULL, 0);
#else	
	if(pthread_create(&_threadId, NULL, WorkerThread::threadFunc,  (void*)this)!= 0) 
	{
		fprintf(stderr,"pthread_create error");
	}
#endif
}

bool WorkerThread::Join() 
{
#if defined( WIN32 )
	int i = 0;

	while(true)
	{
		++i;
		DWORD dw = WaitForSingleObject(_threadId, INFINITE);  

		switch (dw)
		{
		case WAIT_OBJECT_0:
			return true;
		case WAIT_TIMEOUT:
			break;
		case WAIT_FAILED:
		default:
			return false;
		};
	}
#else
	void* status;
	if(pthread_join(_threadId, &status))
		return false;
	return true;
#endif
}

#if defined( WIN32 )
unsigned __stdcall WorkerThread::threadFunc(void *ctx)
#else	
void* WorkerThread::threadFunc(void* ctx)
#endif
{
	WorkerThread * thr = static_cast<WorkerThread*>(ctx);
	//LOG_ERROR("thread id:%d,running\n",thr->_threadId);
	thr->Run();
	//LOG_ERROR("thread id:%d,dead\n",thr->_threadId);
	return NULL;
}

void WorkerThread::Wait() 
{
#if defined( WIN32 )
	WaitForSingleObject(_cond, INFINITE); 
	ResetEvent(_cond);
#else		
	lock();
	pthread_cond_wait(&_cond, &_mutex);
	unlock();
#endif
}

void WorkerThread::Wakeup() 
{
	THREAD_SINGNAL_SET(_cond);
}

void Sleep(int wait)
{
#if defined( WIN32 )
		::Sleep(wait);
#else
		usleep(wait*1000);
#endif
}

}