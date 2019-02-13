#include "ServerApp.h"
#include "util/format.h"
#include "time/Timestamp.h"
#include "logger/Logger.h"
#include "lualib/LuaUtil.h"

extern "C" int luaopen_rapidjson(lua_State* L);
extern "C" int luaopen_lfs(lua_State* L);
extern "C" int luaopen_trie(lua_State* L);
int luaopen_co_stats(lua_State* L);


ServerApp::ServerApp(Network::EventPoller* poller) {
	poller_ = poller;
	lua_ = new LuaFish();
	timer_ = new Timer();
	queue_ = new TaskQueue();
	now_ = ::Now();
}

ServerApp::~ServerApp() {
	delete lua_;
	delete queue_;
	delete timer_;
}

int ServerApp::Init(std::string boot) {
	using namespace std::placeholders;

	timer_->SetCallback(std::bind(&ServerApp::OnUpate, this, _1, _2));
	timer_->Start(poller_, 0.01f, 0.01f);

	lua_->Init(this);

	lua_->SetPath("../../script/?.lua;");

	lua_->Require("fish", LuaFish::Register);
	lua_->Require("util", LuaUtil::Register);
	lua_->Require("json", luaopen_rapidjson);
	lua_->Require("lfs", luaopen_lfs);
	lua_->Require("costats", luaopen_co_stats);
	lua_->Require("trie", luaopen_trie);
	
	lua_->DoFile("../../script/server.lua");
	
	OOLUA::Script& script = lua_->GetScript();
	if (!script.call("ServerInit", boot)) {
		LOG_ERROR(fmt::format("serverInit error:{}",OOLUA::get_last_error(script)));
		return -1;
	}

	return 0;
}

int ServerApp::Fina() {
	OOLUA::Script& script = lua_->GetScript();
	if (!script.call("ServerFina")) {
		LOG_ERROR(fmt::format("serverFina error:{}",OOLUA::get_last_error(script)));
		return -1;
	}
	return 0;
}

int ServerApp::Stop() {
	poller_->Break();
	return 0;
}

int ServerApp::Run() {
	poller_->Process();
	return 0; 
}

void ServerApp::OnUpate(Timer* timer, void* userdata) {
	queue_->Prepare();

	MainTask* task = NULL;
	while ((task = queue_->Pop()) != NULL)  {
		task->MainDo();
		delete task;
	}

	now_ = ::TimeStamp() / 1000;
	
	OOLUA::Script& script = lua_->GetScript();
	if (!script.call("ServerUpdate",now_)) {
		LOG_ERROR(fmt::format("serverUpdate error:{}",OOLUA::get_last_error(script)));
	}
}

uint64_t ServerApp::Now() {
	return now_;
}

LuaFish* ServerApp::Lua() {
	return lua_;
}

Network::EventPoller* ServerApp::Poller() {
	return poller_;
}

TaskQueue* ServerApp::GetQueue() {
	return queue_;
}


