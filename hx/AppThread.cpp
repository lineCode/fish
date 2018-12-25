#include "AppThread.h"


AppThread::AppThread(ServerApp* app):_app(app)
{
}


AppThread::~AppThread(void)
{
}

void AppThread::Run()
{
	this->_app->Run();
}
