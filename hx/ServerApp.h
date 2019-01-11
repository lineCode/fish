#ifndef SERVERAPP_H
#define SERVERAPP_H

//#include "boost/function.hpp"
#include "network/EventPoller.h"
#include "util/MemoryStream.h"
#include "util/ObjectPool.h"
#include "network/Channel.h"
#include "lualib/LuaFish.h"
#include "time/Timer.h"
#include "TaskQueue.h"
#include "Typedef.h"

class ServerApp {
public:
	ServerApp(Network::EventPoller* poller);

	virtual ~ServerApp(void);

	virtual int Init(std::string& boot);

	virtual int Fina();

	virtual int Stop();

	virtual int Run();

	virtual void OnUpate(Timer* timer, void* userdata);

	virtual uint64_t Now();

	virtual LuaFish* Lua();

	virtual Network::EventPoller* Poller();

protected:
	Network::EventPoller* poller_;
	LuaFish* lua_;
	Timer* timer_;
	TaskQueue* queue_;
	uint64_t now_;
};

#endif