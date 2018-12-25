#include "ServerApp.h"
#include "lualib/LuaAcceptor.h"
#include "lualib/LuaConnector.h"
#include "lualib/LuaSocket.h"
#include "lualib/LuaServer.h"
#include "lualib/LuaTimer.h"
#include "lualib/LuaMongo.h"
#include "lualib/LuaZeropack.h"
#include "lualib/LuaMessageHelper.h"
#include "lualib/LuaProf.h"
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
	lua_->Require("Acceptor", LuaAcceptor::Register);
	lua_->Require("Connector", LuaConnector::Register);
	lua_->Require("SocketCore", LuaSocket::Register);
	lua_->Require("Server", LuaServer::Register);
	lua_->Require("Timer", LuaTimer::Register);
	lua_->Require("MongoCore", LuaMongo::Register);
	lua_->Require("Zeropack", LuaZeropack::Register);
	lua_->Require("MessageHelper", LuaMessageHelper::Register);
	lua_->Require("LuaProf", LuaProf::luaopen_prof);
	return 0;
}

int ServerApp::Fina()
{
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


