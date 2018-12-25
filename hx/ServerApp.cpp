#include "ServerApp.h"
#include "util/format.h"
#include "time/Timestamp.h"

#include "Logger.h"

ServerApp::ServerApp()
{
	poller_ = new Network::EventPoller();
	lua_ = new LuaFish();
	now_ = ::Now();
	state_ = AppRun;
}

ServerApp::~ServerApp()
{
	delete lua_;
	delete poller_;
}

int ServerApp::Init()
{
	StartTimer(poller_, 0.01, 0.01);

	lua_->Init(this);

#if defined (WIN32)
	lua_->LuaPath("..\\script\\?.lua;");
	lua_->LuaPath("..\\lib\\3rd\\pbc\\binding\\lua\\?.lua;");
	lua_->LuaPath("..\\lib\\3rd\\luaprofiler\\analyzer\\?.lua;");
#else
	lua_->LuaPath("../script/?.lua;");
	lua_->LuaPath("../lib/3rd/pbc/binding/lua/?.lua;");
	lua_->LuaPath("../lib/3rd/luaprofiler/analyzer/?.lua;");
#endif
	
	lua_->Require("Core", LuaFish::Register);

	OOLUA::Script& script = lua_->GetScript();
	script.call("serverInit");

	return 0;
}

int ServerApp::Fina()
{
	OOLUA::Script& script = lua_->GetScript();
	script.call("serverFina");
	return 0;
}

int ServerApp::Stop()
{
	state_ = AppStop;
	return 0;
}

int ServerApp::Run()
{
	while (state_ == AppRun)
	{
		poller_->Process();
	}
	return 0; 
}

void ServerApp::HandleTimeout()
{
	now_ = ::Now();
	OOLUA::Script& script = lua_->GetScript();
	script.call("serverUpdate", now_);
}

LuaFish* ServerApp::Lua()
{
	return lua_;
}

Network::EventPoller* ServerApp::Poller()
{
	return poller_;
}

uint64 ServerApp::Now()
{
	return now_;
}


