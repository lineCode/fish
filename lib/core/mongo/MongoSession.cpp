#include "MongoSession.h"
#include "MongoCursor.h"

MongoSession::MongoSession(Network::EventPoller * poller,int fd):MongoBase(poller,fd)
{
	_app = NULL;
}

MongoSession::~MongoSession(void)
{

}

int MongoSession::Init()
{
	return MongoBase::Init();
}

int MongoSession::Fina()
{
	return 0;
}

int MongoSession::Dispatcher(int reponseId,char* data,int size)
{
	if (MongoBase::Dispatcher(reponseId,data,size) < 0)
		return _app->LuaManager()->DispatchMongo(_fd,reponseId,data,size);
	
	return -1;
}

void MongoSession::SetApp(ServerApp* app)
{
	_app = app;
}