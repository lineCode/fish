#ifndef MONGOSESSION_H
#define MONGOSESSION_H

#include "MongoBase.h"

class MongoSession : public MongoBase
{
public:
	MongoSession(Network::EventPoller * poller,int fd);

	virtual ~MongoSession(void);

	int Init();

	int Fina();

	int Dispatcher(int reponseId,char* data,int size);

	void SetApp(ServerApp* app);

private:
	ServerApp* _app;
};

#endif