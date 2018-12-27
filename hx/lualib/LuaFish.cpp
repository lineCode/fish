#include "LuaFish.h"
#include "../ServerApp.h"
#include "../Logger.h"
#include "../util/MemoryStream.h"

#include "../time/Timestamp.h"


LuaFish::LuaFish(void) :script_() {
}


LuaFish::~LuaFish(void) {
}

lua_State* LuaFish::LuaState() {
	return script_.state();
}

OOLUA::Script& LuaFish::GetScript() {
	return script_;
}

int LuaFish::Init(ServerApp* app) {
	lua_pushlightuserdata(LuaState(), app);
	lua_setfield(LuaState(), LUA_REGISTRYINDEX, "app");
	return 0;
}

int LuaFish::LoadFile(std::string& file) {
 	if (!script_.load_file(file)) {
 		LOG_ERROR(fmt::format("do file:{} error:{}",file,OOLUA::get_last_error(script_)));
		return -1;
 	}
 	return 0;
}

int LuaFish::DoFile(const char* f) {
	std::string file(f);
	return DoFile(file);
}

int LuaFish::DoFile(std::string& file) {
	if (!script_.run_file(file)) {
		LOG_ERROR(fmt::format("do file:{} error:{}",file,OOLUA::get_last_error(script_)));
		return -1;
	}
	return 0;
}

int LuaFish::CallFunc(const char* module, const char* method) {
	lua_State* L = script_.state();

	lua_getglobal(L, module);
	if ( lua_isnil(L, -1) ) {
		lua_pop(L, 1);
		LOG_ERROR(fmt::format("call func error:no such module:{}", module));
		return -1;
	}

	lua_getfield(L, -1, method);
	if ( lua_isnil(L, -1) ) {
		lua_pop(L, 1);
		LOG_ERROR(fmt::format("call func error:no such method:{} in module:{}", method, module));
		return -1;
	}

	if (LUA_OK != lua_pcall(L, 0, 0, 0)) {
		LOG_ERROR(fmt::format("call func:{}:{} error:{}", module, method, OOLUA::get_last_error(script_)));
		return -1;
	}

	return 0;

}
int LuaFish::CallFunc(std::string& module, std::string& method) {
	return CallFunc(module.c_str(), method.c_str());
}

void LuaFish::LuaPath(const char* path) {
	std::string fullpath(path);

	lua_getglobal(script_.state(), "package");
	lua_getfield(script_.state(), -1, "path");

	const char* opath = lua_tostring(script_.state(), -1);
	fullpath.append(opath);

	lua_pop(script_.state(), 1);
	
	lua_pushstring(script_.state(), fullpath.c_str());
	lua_setfield(script_.state(), -2, "path");
}

void LuaFish::Require(const char* module, int (*func)(lua_State*)) {
	luaL_requiref(LuaState(), module, func,0);
}

LuaTimer* LuaFish::GetTimer(int timerId) {
	std::map<int, LuaTimer*>::iterator iter = timerMgr_.find(timerId);
	if (iter == timerMgr_.end()) {
		return NULL;
	}
	return iter->second;
}

void LuaFish::BindTimer(int timerId, LuaTimer* timer) {
	timerMgr_[timerId] = timer;
}

int LuaFish::DeleteTimer(int timerId) {
	std::map<int, LuaTimer*>::iterator iter = timerMgr_.find(timerId);
	if (iter == timerMgr_.end()) {
		return -1;
	}
	LuaTimer* timer = iter->second;
	timerMgr_.erase(iter);
	delete timer;
	return 0;
}

void LuaFish::OnTimeout(LuaTimer* timer, void* userdata) {
	int timerId = (int)(intptr_t)userdata;

	lua_rawgeti(LuaState(), LUA_REGISTRYINDEX, timerId);

	if (LUA_OK != lua_pcall(LuaState(), 0, 0, 0)) {
		LOG_ERROR(fmt::format("OnTimeout error:{}", lua_tostring(LuaState(), -1)));
	}
	if (!timer->IsActive()) {
		assert(DeleteTimer() == 0);
		luaL_unref(LuaState(), LUA_REGISTRYINDEX, timerId);
	}
}

int LuaFish::Register(lua_State* L)
{
	luaL_checkversion(L);

	luaL_Reg l[] = {
		{ "Log", LuaFish::Log },
		{ "Now", LuaFish::Now },
		{ "Timestamp", LuaFish::Timestamp},
		{ "TimestampToSecond", LuaFish::TimestampToSecond},
		{ "StartTimer", LuaFish::StartTimer},
		{ "CancelTimer", LuaFish::CancelTimer},
		{ "Stop", LuaFish::Stop },
		{ NULL, NULL },
	};

	luaL_newlibtable(L, l);

	lua_getfield(L, LUA_REGISTRYINDEX, "app");
	ServerApp *app = (ServerApp*)lua_touserdata(L,-1);
	if (app == NULL) {
		return luaL_error(L, "Init ServerApp context first");
	}

	luaL_setfuncs(L,l,1);

	return 1;
}

int LuaFish::Log(lua_State* L) {
	Logger::GetSingleton().LuaLog(lua_tostring(L,1));
	return 0;
}

int LuaFish::Now(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushnumber(L,app->Now());
	return 1;
}

int LuaFish::Timestamp(lua_State* L) {
	uint64 now = TimeStamp();
	lua_pushnumber(L,now);
	return 1;
}

int LuaFish::TimestampToSecond(lua_State* L) {
	lua_Number ti = lua_tonumber(L,1);
	lua_pushnumber(L,ti/StampPersecond());
	return 1;
}

int LuaFish::StartTimer(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	luaL_checktype(L, 1, LUA_TFUNCTION);

	lua_pushvalue(L, 1);
	int timerId = luaL_ref(L, LUA_REGISTRYINDEX);

	double after = luaL_checknumber(L, 2);
	double repeat = luaL_checknumber(L, 3);

	LuaFish* fish = app->Lua();

	LuaTimer* timer = new LuaTimer();
	timer->SetCallback(std::bind(&LuaFish::OnTimeout, fish, std::placeholders::_1,std::placeholders::_2));
	timer->SetUserdata((void*)(long)timerId);
	timer->StartTimer(app->Poller(), after, repeat);

	fish->BindTimer(timerId, timer);

	lua_pushinteger(L, timerId);
	return 1;
}

int LuaFish::CancelTimer(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

	int timerId = luaL_checkinteger(L, 1);

	if (app->Lua()->DeleteTimer(timerId) == 0) {
		luaL_unref(L, LUA_REGISTRYINDEX, timerId);
	}

	return 0;
}

int LuaFish::Stop(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	app->Stop();
	return 0;
}
