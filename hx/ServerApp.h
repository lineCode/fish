#ifndef SERVERAPP_H
#define SERVERAPP_H

//#include "boost/function.hpp"
#include "network/EventPoller.h"
#include "util/MemoryStream.h"
#include "util/MessageWriter.h"
#include "util/MessageReader.h"
#include "util/ObjectPool.h"
#include "network/Channel.h"
#include "lualib/LuaFish.h"
#include "time/Timer.h"
#include "TaskQueue.h"
#include "Typedef.h"

class MongoSession;

class ServerApp {
public:
	enum AppState{AppRun,AppStop};

public:
	ServerApp(Network::EventPoller* poller);

	virtual ~ServerApp(void);

	virtual int Init(std::string& boot);

	virtual int Fina();

	virtual int Stop();

	virtual int Run();

	virtual void OnUpate(Timer* timer, void* userdata);

	virtual uint64 Now();

	virtual LuaFish* Lua();

	virtual Network::EventPoller* Poller();

protected:
	AppState  state_;
	Network::EventPoller* poller_;
	Timer* timer_;
	LuaFish* lua_;
	TaskQueue* queue_;
	uint64 now_;
};

#endif