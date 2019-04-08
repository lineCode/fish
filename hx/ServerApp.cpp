#include "ServerApp.h"
#include "util/format.h"
#include "time/Timestamp.h"
#include "logger/Logger.h"
#include "lualib/LuaUtil.h"

extern "C" int luaopen_rapidjson(lua_State* L);
extern "C" int luaopen_lfs(lua_State* L);
extern "C" int luaopen_trie(lua_State* L);
extern "C" int luaopen_pto(lua_State* L);
extern "C" int luaopen_dump_core(lua_State* L);
extern "C" int luaopen_nav_core(lua_State* L);
extern "C" int luaopen_pathfinder_core(lua_State* L);

int luaopen_co_stats(lua_State* L);


ServerApp::ServerApp(Network::EventPoller* poller) {
	poller_ = poller;
	lua_ = new LuaFish();
	timer_ = new Timer();
	queue_ = new TaskQueue();
	now_ = ::Now();
	name_ = "";
	stop_ = false;

#ifndef WIN32
	sigUSR1_.set(poller->GetLoop());
	sigUSR1_.set<ServerApp, &ServerApp::OnLuaBreak>(this);
	sigUSR1_.start(SIGUSR1);
#endif

	sigTERM_.set(poller->GetLoop());
	sigTERM_.set<ServerApp, &ServerApp::OnStop>(this);
	sigTERM_.start(SIGTERM);
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
	lua_->Require("pto", luaopen_pto);
	lua_->Require("dump", luaopen_dump_core);
	lua_->Require("nav", luaopen_nav_core);
	lua_->Require("pathfinder", luaopen_pathfinder_core);
	
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

	now_ = ::Now();
	
	OOLUA::Script& script = lua_->GetScript();
	if (!script.call("ServerUpdate",now_)) {
		LOG_ERROR(fmt::format("serverUpdate error:{}",OOLUA::get_last_error(script)));
	}
}

void ServerApp::OnStop() {
	if (stop_) {
		return;
	}
	stop_ = true;

	OOLUA::Script& script = lua_->GetScript();
	if ( !script.call("ServerStop") ) {
		LOG_ERROR(fmt::format("ServerStop error:{}", OOLUA::get_last_error(script)));
	}
}

void ServerApp::OnLuaBreak() {
	LOG_ERROR(std::string("break out lua"));
	lua_->Breakout();
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


