#ifndef SERVERAPP_H
#define SERVERAPP_H


#include "network/EventPoller.h"
#include "util/MemoryStream.h"
#include "util/ObjectPool.h"
#include "network/Channel.h"
#include "lualib/LuaFish.h"
#include "time/Timer.h"
#include "TaskQueue.h"


class ServerApp {
public:
	ServerApp(Network::EventPoller* poller);

	virtual ~ServerApp(void);

	virtual int Init(std::string boot);

	virtual int Fina();

	virtual int Stop();

	virtual int Run();

	virtual void OnUpate(Timer* timer, void* userdata);

	virtual void OnStop();

	virtual void OnLuaBreak();

	virtual uint64_t Now();

	virtual LuaFish* Lua();

	virtual Network::EventPoller* Poller();

	virtual TaskQueue* GetQueue();

	virtual void SetName(std::string name) {
		name_ = name;
	}

	virtual std::string GetName() {
		return name_;
	}
	
protected:
	Network::EventPoller* poller_;
	LuaFish* lua_;
	Timer* timer_;
	TaskQueue* queue_;
	uint64_t now_;
	std::string name_;
	bool stop_;

	ev::sig sigTERM_;
};

#endif