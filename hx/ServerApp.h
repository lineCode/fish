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
#include "Typedef.h"

class MongoSession;

class ServerApp : public Timer
{
public:
	enum AppState{AppRun,AppStop};
public:
	ServerApp(void);
	virtual ~ServerApp(void);

	virtual int Init();

	virtual int Fina();

	virtual int Stop();

	virtual int Run();

	virtual void HandleTimeout();

	virtual LuaFish* Lua();

	virtual Network::EventPoller* Poller();

	virtual uint64 Now();
protected:
	AppState  state_;
	Network::EventPoller * poller_;
	LuaFish* lua_;
	uint64 now_;
};

#endif