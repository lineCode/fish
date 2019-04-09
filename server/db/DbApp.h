#ifndef DBAPP_H
#define DBAPP_H

#include <vector>
#include <map>
#include <string>

#include "ServerApp.h"
#include "network/Acceptor.h"
#include "network/Address.h"
#include "util/Singleton.h"
#include "db/DbThreadPool.h"
#include "document.h" 

class DbApp : public ServerApp , public Singleton<DbApp>
{
public:
	DbApp(Network::EventPoller* poller);

	~DbApp(void);

	virtual int Init(rapidjson::Document& config);

	virtual int Fina();

	virtual DbThreadPool* GetThreadPool();

	static int Register(lua_State* L);

	static int LQuery(lua_State* L);

	static int LExecute(lua_State* L);

private:
	DbThreadPool* dbThreadPool_;
};

#define DB_APP DbApp::GetSingleton()

#endif
