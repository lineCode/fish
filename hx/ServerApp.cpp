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

	lua_->LuaPath("../../script/?.lua;");

	lua_->Require("fish", LuaFish::Register);

	lua_->DoFile("../../script/server.lua");
	
	OOLUA::Script& script = lua_->GetScript();
	if (!script.call("serverInit")) {
		LOG_ERROR(fmt::format("serverInit error:{}",OOLUA::get_last_error(script)));
		return -1;
	}

	return 0;
}

int ServerApp::Fina()
{
	OOLUA::Script& script = lua_->GetScript();
	if (!script.call("serverFina")) {
		LOG_ERROR(fmt::format("serverFina error:{}",OOLUA::get_last_error(script)));
		return -1;
	}


	return 0;
}

int ServerApp::Stop()
{
	poller_->Break();
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
	now_ = ::TimeStamp() / 1000;
	OOLUA::Script& script = lua_->GetScript();
	if (!script.call("serverUpdate",now_)) {
		LOG_ERROR(fmt::format("serverUpdate error:{}",OOLUA::get_last_error(script)));
	}

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


