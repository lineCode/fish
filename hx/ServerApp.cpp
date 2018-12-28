#include "ServerApp.h"
#include "util/format.h"
#include "time/Timestamp.h"
#include "Logger.h"

extern "C" int luaopen_rapidjson(lua_State* L);
extern "C" int luaopen_cjson(lua_State *l);

ServerApp::ServerApp() {
	state_ = AppRun;
	poller_ = new Network::EventPoller();
	timer_ = new Timer();
	lua_ = new LuaFish();
	queue_ = new TaskQueue();
	now_ = ::Now();
}

ServerApp::~ServerApp() {
	delete poller_;
	delete timer_;
	delete lua_;
}

int ServerApp::Init() {
	timer_->SetCallback(std::bind(&ServerApp::OnUpate, this, std::placeholders::_1, std::placeholders::_2));
	timer_->Start(poller_, 0.01, 0.01);

	lua_->Init(this);

	lua_->Require("fish", LuaFish::Register);
	lua_->Require("json", luaopen_rapidjson);
	lua_->Require("cjson", luaopen_cjson);
	
	lua_->SetPath("../../script/?.lua;");

	lua_->DoFile("../../script/server.lua");
	
	OOLUA::Script& script = lua_->GetScript();
	if (!script.call("serverInit")) {
		LOG_ERROR(fmt::format("serverInit error:{}",OOLUA::get_last_error(script)));
		return -1;
	}

	return 0;
}

int ServerApp::Fina() {
	OOLUA::Script& script = lua_->GetScript();
	if (!script.call("serverFina")) {
		LOG_ERROR(fmt::format("serverFina error:{}",OOLUA::get_last_error(script)));
		return -1;
	}
	return 0;
}

int ServerApp::Stop() {
	poller_->Break();
	state_ = AppStop;
	return 0;
}

int ServerApp::Run() {
	while (state_ == AppRun) {
		poller_->Process();
	}
	return 0; 
}

void ServerApp::OnUpate(Timer* timer, void* userdata) {
	Task* task = NULL;
	while ((task = queue_->Pop()) != NULL)  {
		task->Do();
	}

	now_ = ::TimeStamp() / 1000;
	
	OOLUA::Script& script = lua_->GetScript();
	if (!script.call("serverUpdate",now_)) {
		LOG_ERROR(fmt::format("serverUpdate error:{}",OOLUA::get_last_error(script)));
	}
}

uint64 ServerApp::Now() {
	return now_;
}

LuaFish* ServerApp::Lua() {
	return lua_;
}

Network::EventPoller* ServerApp::Poller() {
	return poller_;
}




