#ifndef APPTHREAD_H
#define APPTHREAD_H
#include "thread/Thread.h"
#include "ServerApp.h"

class AppThread : public Thread::WorkerThread
{
public:
	AppThread(ServerApp* app);
	~AppThread(void);

	void Run();
protected:
	ServerApp* _app;
};

#endif