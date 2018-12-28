#include "LuaFish.h"
#include "ServerApp.h"
#include "Logger.h"
#include "network/Acceptor.h"
#include "network/Connector.h"
#include "util/MemoryStream.h"
#include "time/Timestamp.h"


enum {
	eCHANNEL_DATA = 1,
	eCHANNEL_CLOSE,
	eCHANNEL_ERROR
};

LuaFish::LuaFish(void) :script_(),timerPool_("timer") {
	timerStep_ = 0;
}

LuaFish::~LuaFish(void) {
}

OOLUA::Script& LuaFish::GetScript() {
	return script_;
}

int LuaFish::Init(ServerApp* app) {
	lua_State* L = script_.state();
	lua_pushlightuserdata(L, app);
	lua_setfield(L, LUA_REGISTRYINDEX, "app");
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

void LuaFish::SetPath(const char* path) {
	lua_State* L = script_.state();
	std::string fullpath(path);

	lua_getglobal(L, "package");
	lua_getfield(L, -1, "path");

	const char* opath = lua_tostring(L, -1);
	fullpath.append(opath);

	lua_pop(L, 1);
	
	lua_pushstring(L, fullpath.c_str());
	lua_setfield(L, -2, "path");
}

void LuaFish::Require(const char* module, int (*func)(lua_State*)) {
	luaL_requiref(script_.state(), module, func,0);
}

uint64_t LuaFish::AllocTimer(Timer*& timer) {
	uint64_t timerId = timerStep_++;
	timerPool_.Pop(timer);
	timerMgr_[timerId] = timer;
	return timerId;
}

int LuaFish::DeleteTimer(uint64_t timerId) {
	std::map<uint64_t, Timer*>::iterator iter = timerMgr_.find(timerId);
	if (iter == timerMgr_.end()) {
		return -1;
	}

	Timer* timer = iter->second;
	timer->Cancel();
	timerPool_.Push(timer);
	timerMgr_.erase(iter);
	return 0;
}

void LuaFish::OnTimeout(Timer* timer, uint64_t timerId, void* userdata) {
	int reference = (int)(intptr_t)userdata;

	lua_State* L = script_.state();

	lua_rawgeti(L, LUA_REGISTRYINDEX, reference);

	if (LUA_OK != lua_pcall(L, 0, 0, 0)) {
		LOG_ERROR(fmt::format("OnTimeout error:{}", lua_tostring(L, -1)));
	}

	if (!timer->IsActive()) {
		std::map<uint64_t, Timer*>::iterator iter = timerMgr_.find(timerId);
		Timer* timer = iter->second;
		timer->Cancel();
		timerPool_.Push(timer);
		timerMgr_.erase(iter);
		luaL_unref(L, LUA_REGISTRYINDEX, reference);
	}
}

void LuaFish::OnAccept(int fd, Network::Addr& addr, void* userdata) {
	lua_State* L = script_.state();

	int callback = (int)(intptr_t)userdata;
	close(fd);

	lua_rawgeti(L, LUA_REGISTRYINDEX, callback);
	lua_pushinteger(L, fd);
	lua_pushstring(L, addr.ToStr().c_str());

	if (LUA_OK != lua_pcall(L, 2, 0, 0)) {
		LOG_ERROR(fmt::format("OnAccept error:{}", lua_tostring(L, -1)));
	}
}

void LuaFish::OnConnect(int fd, const char* reason, void* userdata) {
	lua_State* L = script_.state();

	int callback = (int)(intptr_t)userdata;
	close(fd);

	lua_rawgeti(L, LUA_REGISTRYINDEX, callback);

	int numArgs;
	if (fd < 0) {
		lua_pushboolean(L, 0);
		lua_pushstring(L, reason);
		numArgs = 2;
	} else {
		lua_pushinteger(L, fd);
		numArgs = 1;
	}

	if (LUA_OK != lua_pcall(L, numArgs, 0, 0)) {
		LOG_ERROR(fmt::format("OnConnect error:{}", lua_tostring(L, -1)));
	}
}

void LuaFish::OnData(int channel, int callback, char* data, size_t size) {

}

void LuaFish::OnError(int channel, int callback, const char* reason) {

}


extern "C" int luaseri_pack(lua_State*);
extern "C" int luaseri_unpack(lua_State*);

int LuaFish::Register(lua_State* L)
{
	luaL_checkversion(L);

	luaL_Reg l[] = {
		{ "Stop", LuaFish::Stop },
		{ "Log", LuaFish::Log },
		{ "Now", LuaFish::Now },
		{ "Timestamp", LuaFish::Timestamp},
		{ "TimestampToSecond", LuaFish::TimestampToSecond},
		{ "Pack", luaseri_pack},
		{ "UnPack", luaseri_unpack},
		{ "Timer", LuaFish::TimerStart},
		{ "Listen", LuaFish::AcceptorListen},
		{ "Connect", LuaFish::ConnectorConnect},
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

int LuaFish::Stop(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	app->Stop();
	return 0;
}

int LuaFish::Log(lua_State* L) {
	Logger::GetSingleton()->LuaLog(lua_tostring(L,1));
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

int LuaFish::TimerStart(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

	double after = luaL_checknumber(L, 1);
	double repeat = luaL_checknumber(L, 2);

	luaL_checktype(L, 3, LUA_TFUNCTION);
	int reference = luaL_ref(L, LUA_REGISTRYINDEX);

	LuaFish* fish = app->Lua();

	Timer* timer = NULL;
	uint64_t timerId = fish->AllocTimer(timer);

	timer->SetCallback(std::bind(&LuaFish::OnTimeout, fish, std::placeholders::_1, timerId, std::placeholders::_2));
	timer->SetUserdata((void*)(long)reference);
	timer->Start(app->Poller(), after, repeat);

	lua_pushinteger(L, timerId);
	return 1;
}

int LuaFish::TimerCancel(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	uint64_t timerId = luaL_checkinteger(L, 1);
	lua_pushboolean(L, app->Lua()->DeleteTimer(timerId) == 0);
	return 1;
}

int LuaFish::AcceptorListen(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

	const char* ip = luaL_checkstring(L, 1);
	int port = luaL_checkinteger(L, 2);
	luaL_checktype(L, 3, LUA_TFUNCTION);
	int callback = luaL_ref(L, LUA_REGISTRYINDEX);

	void* ud = lua_newuserdata(L, sizeof(Network::Acceptor));

	if (luaL_newmetatable(L, "metaAcceptor")) {
        const luaL_Reg meta[] = {
            { "Stop", LuaFish::AcceptorClose },
			{ NULL, NULL },
        };
        luaL_newlib(L, meta);
        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, LuaFish::AcceptorRelease);
        lua_setfield(L, -2, "__gc");
    }
    lua_setmetatable(L, -2);

	Network::Acceptor* acceptor = new(ud) Network::Acceptor(app->Poller());

	acceptor->SetCallback(std::bind(&LuaFish::OnAccept, app->Lua(), std::placeholders::_1, std::placeholders::_2,std::placeholders::_3));
	acceptor->SetUserdata((void*)(long)callback);

	Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);

	acceptor->Listen(addr);

	return 1;
}

int LuaFish::AcceptorClose(lua_State* L) {
	Network::Acceptor* acceptor = (Network::Acceptor*)lua_touserdata(L, 1);
	lua_pushboolean(L, acceptor->Close() == 0);
	return 1;
}

int LuaFish::AcceptorRelease(lua_State* L) {
	Network::Acceptor* acceptor = (Network::Acceptor*)lua_touserdata(L, 1);
	int callback = (int)(intptr_t)acceptor->GetUserdata();
	luaL_unref(L, LUA_REGISTRYINDEX, callback);
	acceptor->~Acceptor();
	return 0;
}

int LuaFish::ConnectorConnect(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

	const char* ip = luaL_checkstring(L, 1);
	int port = luaL_checkinteger(L, 2);
	luaL_checktype(L, 3, LUA_TFUNCTION);
	int callback = luaL_ref(L, LUA_REGISTRYINDEX);

	void* ud = lua_newuserdata(L, sizeof(Network::Connector));

	if (luaL_newmetatable(L, "metaConnector")) {
        const luaL_Reg meta[] = {
            { "Stop", LuaFish::ConnectorClose },
			{ NULL, NULL },
        };
        luaL_newlib(L, meta);
        lua_setfield(L, -2, "__index");

        lua_pushcfunction(L, LuaFish::ConnectorRelease);
        lua_setfield(L, -2, "__gc");
    }
    lua_setmetatable(L, -2);

    Network::Connector* connector = new(ud) Network::Connector(app->Poller());

    connector->SetCallback(std::bind(&LuaFish::OnConnect, app->Lua(), std::placeholders::_1, std::placeholders::_2,std::placeholders::_3));
	connector->SetUserdata((void*)(long)callback);

	Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);

	connector->Connect(addr);

	return 1;
}

int LuaFish::ConnectorClose(lua_State* L) {
	Network::Connector* connector = (Network::Connector*)lua_touserdata(L, 1);
	lua_pushboolean(L, connector->Close() == 0);
	return 1;
}

int LuaFish::ConnectorRelease(lua_State* L) {
	Network::Connector* connector = (Network::Connector*)lua_touserdata(L, 1);

	int callback = (int)(intptr_t)connector->GetUserdata();
	luaL_unref(L, LUA_REGISTRYINDEX, callback);

	connector->~Connector();

	return 0;
}

int LuaFish::BindChannel(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	int fd = luaL_checkinteger(L, 1);
	uint32_t header = luaL_checkinteger(L, 2);

	
}
