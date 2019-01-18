#ifndef DBAPP_H
#define DBAPP_H

#include <vector>
#include <map>
#include <string>

#include "ServerApp.h"
#include "network/Acceptor.h"
#include "network/Address.h"
#include "db/DbThreadPool.h"

class DbApp : public ServerApp
{
public:
	DbApp(Network::EventPoller* poller);

	~DbApp(void);

	virtual int Init(rapidjson::Document& config);

	virtual int Fina();

private:
	DbThreadPool* dbThreadPool_;

};

#endif
