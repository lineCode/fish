#include "LuaFish.h"
#include "ServerApp.h"
#include "logger/Logger.h"
#include "network/Acceptor.h"
#include "network/Connector.h"
#include "util/MemoryStream.h"
#include "time/Timestamp.h"
#include "LuaChannel.h"
#include "LuaHttpChannel.h"

#ifdef MEM_USE_TC
#include "tcmalloc.h"
#include "heap-profiler.h"
#include "profiler.h"
#include "malloc_extension.h"
#endif

extern int G_TIMEDIFF;

using namespace std::placeholders;

LuaFish::LuaFish(void) :script_() {
	timerStep_ = 0;
}

LuaFish::~LuaFish(void) {
}

OOLUA::Script& LuaFish::GetScript() {
	return script_;
}

int LuaFish::Init(ServerApp* app) {
	lua_State* L = script_.state();
	luaL_openlibs(L);

	lua_pushlightuserdata(L, app);
	lua_setfield(L, LUA_REGISTRYINDEX, "app");

	{
		lua_getglobal(L, "os");

		lua_pushlightuserdata(L, app);
		lua_getglobal(L, "os");
		lua_getfield(L, -1, "time");
		lua_pushcclosure(L, LuaFish::OsTime, 2);

		lua_setfield(L, -2, "time");
	}

	{
		lua_getglobal(L, "os");

		lua_pushlightuserdata(L, app);
		lua_getglobal(L, "os");
		lua_getfield(L, -1, "date");
		lua_pushcclosure(L, LuaFish::OsDate, 2);

		lua_setfield(L, -2, "date");
	}
	
	const luaL_Reg metaAcceptor[] = {
        { "Stop", LuaFish::AcceptorClose },
		{ NULL, NULL },
    };
    CreateMetaTable("metaAcceptor", metaAcceptor, LuaFish::AcceptorRelease);

    const luaL_Reg metaConnector[] = {
        { "Stop", LuaFish::ConnectorClose },
		{ NULL, NULL },
    };
	CreateMetaTable("metaConnector", metaConnector, LuaFish::ConnectorRelease);

    const luaL_Reg metaChannel[] = {
        { "Read", LuaChannel::LuaRead },
		{ "Write", LuaChannel::LuaWrite },
		{ "Close", LuaChannel::LuaClose },
		{ NULL, NULL },
    };
	CreateMetaTable("metaChannel", metaChannel, LuaChannel::LuaRelease);

    const luaL_Reg metaHttpChannel[] = {
		{ "GetUrl", LuaHttpChannel::LuaGetUrl },
		{ "GetHeader", LuaHttpChannel::LuaGetHeader },
		{ "GetContent", LuaHttpChannel::LuaGetContent },
		{ "SetHeader", LuaHttpChannel::LuaSetReplyHeader },
		{ "Reply", LuaHttpChannel::LuaReply },
		{ "Close", LuaHttpChannel::LuaClose },
		{ NULL, NULL },
    };
	CreateMetaTable("metaHttpChannel", metaHttpChannel, LuaHttpChannel::LuaRelease);
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

OOLUA::Table LuaFish::CreateGlobalTable(const char* name) {
	lua_State* L = (lua_State*)script_;

	lua_newtable(L);
	lua_setglobal(L, name);
	return OOLUA::Table(L, name);
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
	luaL_requiref(script_.state(), module, func, 1);
}

void LuaFish::Breakout() {
	LUA_BREAKOUT = 1;
}

void LuaFish::CreateMetaTable(const char* name, const luaL_Reg meta[], lua_CFunction gc) {
	lua_State* L = script_.state();

	if (luaL_newmetatable(L, name)) {
        luaL_newlib(L, meta);
        lua_setfield(L, -2, "__index");

        if (gc) {
        	lua_pushcfunction(L, gc);
        	lua_setfield(L, -2, "__gc");
        }
    }
}

uint64_t LuaFish::AllocTimer(Timer*& timer) {
	uint64_t timerId;
	for(;;) {
		timerId = timerStep_++;
		TimerMap::iterator iter = timerMgr_.find(timerId);
		if (iter == timerMgr_.end()) {
			timer = Timer::AssignTimer();
			timerMgr_[timerId] = timer;
			break;
		}
	}
	return timerId;
}

int LuaFish::DeleteTimer(uint64_t timerId) {
	TimerMap::iterator iter = timerMgr_.find(timerId);
	if (iter == timerMgr_.end()) {
		return -1;
	}

	Timer* timer = iter->second;
	timer->Cancel();
	timerMgr_.erase(iter);

	Timer::ReclaimTimer(timer);
	return 0;
}

Timer* LuaFish::GetTimer(uint64_t timerId) {
	TimerMap::iterator iter = timerMgr_.find(timerId);
	if (iter == timerMgr_.end()) {
		return NULL;
	}
	return iter->second;
}

void LuaFish::OnTimeout(Timer* timer, uint64_t timerId, void* userdata) {
	int reference = (int)(intptr_t)userdata;

	lua_State* L = script_.state();

	lua_rawgeti(L, LUA_REGISTRYINDEX, reference);

	if (LUA_OK != lua_pcall(L, 0, 0, 0)) {
		LOG_ERROR(fmt::format("OnTimeout error:{}", lua_tostring(L, -1)));
	}

	if (!timer->IsActive()) {
		DeleteTimer(timerId);
		luaL_unref(L, LUA_REGISTRYINDEX, reference);
	}
}

void LuaFish::OnAccept(int fd, Network::Addr& addr, void* userdata) {
	lua_State* L = script_.state();

	int callback = (int)(intptr_t)userdata;

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

extern "C" int luaseri_pack(lua_State*);
extern "C" int luaseri_unpack(lua_State*);

int Ref(lua_State* L, int index, int type) {
	luaL_checktype(L, index, type);
	return luaL_ref(L, LUA_REGISTRYINDEX);
}

int LuaFish::Register(lua_State* L) {
	luaL_checkversion(L);

	luaL_Reg methods[] = {
		{ "GetAppName", LuaFish::GetAppName },
		{ "Stop", LuaFish::Stop },
		{ "WriteLog", LuaFish::WriteLog },
		{ "SendLog", LuaFish::SendLog },
		{ "Now", LuaFish::Now },
		{ "Timestamp", LuaFish::Timestamp},
		{ "SetTimeDiff", LuaFish::SetTimeDiff},
		{ "Dump", LuaFish::Dump},
		{ "GetMemory", LuaFish::GetMemory},
		{ "FreeMemory", LuaFish::FreeMemory},
		{ "CPUProfilerStart", LuaFish::CPUProfilerStart},
		{ "CPUProfilerStop", LuaFish::CPUProfilerStop},
		{ "HeapProfilerStart", LuaFish::HeapProfilerStart},
		{ "HeapProfilerStop", LuaFish::HeapProfilerStop},
		{ "HeapProfilerDump", LuaFish::HeapProfilerDump},
		{ "Pack", luaseri_pack},
		{ "UnPack", luaseri_unpack},
		{ "StartTimer", LuaFish::TimerStart},
		{ "CancelTimer", LuaFish::TimerCancel},
		{ "Listen", LuaFish::Listen},
		{ "Connect", LuaFish::Connect},
		{ "Bind", LuaFish::BindChannel},
		{ "BindHttp", LuaFish::BindHttpChannel },
		{ NULL, NULL },
	};

	luaL_newlibtable(L, methods);

	lua_getfield(L, LUA_REGISTRYINDEX, "app");
	ServerApp *app = (ServerApp*)lua_touserdata(L, -1);
	if (app == NULL) {
		return luaL_error(L, "Init ServerApp context first");
	}
	luaL_setfuncs(L, methods, 1);

	return 1;
}

int LuaFish::GetAppName(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	std::string name = app->GetName();
	lua_pushlstring(L, name.c_str(), name.size());
	return 1;
}

int LuaFish::Stop(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	app->Stop();
	return 0;
}

int LuaFish::WriteLog(lua_State* L) {
	const char* file = luaL_checkstring(L, 1);
	const char* source = luaL_checkstring(L, 2);
	int line = luaL_checkinteger(L, 3);
	int level = luaL_checkinteger(L, 4);
	double time = luaL_checknumber(L, 5);
	const char* log = luaL_checkstring(L, 6);
	LOGGER->Write(file, source, line, (Logger::LogLv)level, time, log);
	return 0;
}

int LuaFish::SendLog(lua_State* L) {
	const char* file = luaL_checkstring(L, 1);
	void* ud = lua_touserdata(L, 2);
	size_t size = luaL_checkinteger(L, 3);
	LOGGER->Write(file, ud, size);
	return 0;
}

int LuaFish::Now(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushinteger(L,app->Now());
	return 1;
}

int LuaFish::Timestamp(lua_State* L) {
	lua_pushnumber(L, GetTimeMillis() / 1000);
	return 1;
}

int LuaFish::SetTimeDiff(lua_State* L) {
	int timeDiff = luaL_checkinteger(L, 1);
	G_TIMEDIFF = timeDiff;
	return 0;
}

int LuaFish::OsTime(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	if ( lua_isnoneornil(L, 1) ) {
		lua_pushinteger(L, app->Now());
		return 1;
	}

	lua_pushvalue(L, lua_upvalueindex(2));
	lua_pushvalue(L, 1);

	if ( lua_pcall(L, 1, 1, 0) != LUA_OK ) {
		luaL_error(L, lua_tostring(L, -1));
	}
	return 1;
}

int LuaFish::OsDate(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	if ( lua_isnoneornil(L, 1) ) {
		lua_pushinteger(L, app->Now());
	}

	lua_pushvalue(L, lua_upvalueindex(2));
	lua_pushvalue(L, 1);

	if ( lua_pcall(L, 1, 1, 0) != LUA_OK ) {
		luaL_error(L, lua_tostring(L, -1));
	}
	return 1;
}

int LuaFish::Dump(lua_State* L) {
	Timer::GetPool().Dump();
	return 0;
}

int LuaFish::GetMemory(lua_State* L) {
#ifndef MEM_USE_TC 
	return 0;
#else
	size_t allocatedBytes;
	MallocExtension::instance()->GetNumericProperty("generic.current_allocated_bytes", &allocatedBytes);
	lua_pushinteger(L, allocatedBytes);
	return 1;
#endif
}

int LuaFish::FreeMemory(lua_State* L) {
#ifndef MEM_USE_TC 
	return 0;
#else
	MallocExtension::instance()->ReleaseFreeMemory();
	return 0;
#endif
}

int LuaFish::CPUProfilerStart(lua_State* L) {
#ifndef MEM_USE_TC 
	return 0;
#else
	const char* name = luaL_checkstring(L, 1);
	ProfilerStart(name);
	return 0;
#endif
}

int LuaFish::CPUProfilerStop(lua_State* L) {
#ifndef MEM_USE_TC 
	return 0;
#else
	ProfilerStop();
	return 0;
#endif
}

int LuaFish::HeapProfilerStart(lua_State* L) {
#ifndef MEM_USE_TC 
	return 0;
#else
	const char* prefix = luaL_checkstring(L, 1);
	::HeapProfilerStart(prefix);
	return 0;
#endif
}

int LuaFish::HeapProfilerStop(lua_State* L) {
#ifndef MEM_USE_TC
	return 0;
#else
	::HeapProfilerStop();
	return 0;
#endif
}

int LuaFish::HeapProfilerDump(lua_State* L) {
#ifndef MEM_USE_TC 
	return 0;
#else
	const char* reason = luaL_checkstring(L, 1);
	::HeapProfilerDump(reason);
	return 0;
#endif
}

int LuaFish::TimerStart(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

	double after = luaL_checknumber(L, 1);
	double repeat = luaL_checknumber(L, 2);

	int reference = Ref(L, 3, LUA_TFUNCTION);

	LuaFish* fish = app->Lua();

	Timer* timer = NULL;
	uint64_t timerId = fish->AllocTimer(timer);

	timer->SetCallback(std::bind(&LuaFish::OnTimeout, fish, _1, timerId, _2));
	timer->SetUserdata((void*)(long)reference);
	timer->Start(app->Poller(), after, repeat);

	lua_pushinteger(L, timerId);
	return 1;
}

int LuaFish::TimerCancel(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	uint64_t timerId = luaL_checkinteger(L, 1);
	Timer* timer = app->Lua()->GetTimer(timerId);
	if (timer == NULL) {
		luaL_error(L, "no such timer:%d", timerId);
	}

	int reference = (int)(intptr_t)timer->GetUserdata();
	luaL_unref(L, LUA_REGISTRYINDEX, reference);
	lua_pushboolean(L, app->Lua()->DeleteTimer(timerId) == 0);
	return 1;
}

Network::Addr MakeAddr(lua_State* L, int index) {
	luaL_checktype(L, index, LUA_TTABLE);
	lua_getfield(L, index, "file");

	Network::Addr addr;
	if (!lua_isnoneornil(L, -1)) {
#ifndef _WIN32
		addr = Network::Addr::MakeUNIXAddr(luaL_checkstring(L, -1));
#else
		luaL_error(L, "win32 not support unix socket");
#endif
		lua_pop(L, 1);
	} else {
		lua_pop(L, 1);

		lua_getfield(L, index, "ip");
		const char* ip = luaL_checkstring(L, -1);

		lua_getfield(L, index, "port");
		int port = luaL_checkinteger(L, -1);
		
		addr = Network::Addr::MakeIP4Addr(ip, port);

		lua_pop(L, 2);
	}

	return addr;
}

int LuaFish::Listen(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	Network::Addr addr = MakeAddr(L, 1);
	int callback = Ref(L, 2, LUA_TFUNCTION);

	void* userdata = lua_newuserdata(L, sizeof(Network::Acceptor));
	luaL_newmetatable(L, "metaAcceptor");
    lua_setmetatable(L, -2);

	Network::Acceptor* acceptor = new(userdata)Network::Acceptor(app->Poller());

	acceptor->SetCallback(std::bind(&LuaFish::OnAccept, app->Lua(), _1, _2, _3));
	acceptor->SetUserdata((void*)(long)callback);

	if (acceptor->Listen(addr) < 0) {
		return 0;
	}

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

int LuaFish::Connect(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	Network::Addr addr = MakeAddr(L, 1);
	int callback = Ref(L, 2, LUA_TFUNCTION);

	void* userdata = lua_newuserdata(L, sizeof( Network::Connector ));
	luaL_newmetatable(L, "metaConnector");
    lua_setmetatable(L, -2);

	Network::Connector* connector = new(userdata)Network::Connector(app->Poller());

    connector->SetCallback(std::bind(&LuaFish::OnConnect, app->Lua(), _1, _2, _3));
	connector->SetUserdata((void*)(long)callback);

	if (connector->Connect(addr) < 0) {
		return 0;
	}

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

	int error = Ref(L, -1, LUA_TFUNCTION);
	int close = Ref(L, -1, LUA_TFUNCTION);
	int data = Ref(L, -1, LUA_TFUNCTION);

	void* userdata = lua_newuserdata(L, sizeof( LuaChannel ));
	luaL_newmetatable(L, "metaChannel");
    lua_setmetatable(L, -2);

    lua_pushvalue(L, -1);

    int reference = Ref(L, -1, LUA_TUSERDATA);

	LuaChannel* channel = new(userdata)LuaChannel(app->Poller(), fd, app->Lua(), header);

    channel->SetReference(reference);
    channel->SetDataReference(data);
    channel->SetCloseReference(close);
    channel->SetErrorReference(error);

    channel->EnableRead();

    return 1;
}

int LuaFish::BindHttpChannel(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	int fd = luaL_checkinteger(L, 1);
	int callback = Ref(L, 2, LUA_TFUNCTION);

	void* userdata = lua_newuserdata(L, sizeof( LuaHttpChannel ));
	luaL_newmetatable(L, "metaHttpChannel");
	lua_setmetatable(L, -2);

	lua_pushvalue(L, -1);
	int reference = Ref(L, -1, LUA_TUSERDATA);

	LuaHttpChannel* channel = new(userdata)LuaHttpChannel(app->Poller(), fd, app->Lua());

	channel->SetReference(reference);
	channel->SetCallback(callback);

	channel->EnableRead();

	return 1;
}
