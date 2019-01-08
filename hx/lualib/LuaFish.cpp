#include "LuaFish.h"
#include "ServerApp.h"
#include "logger/Logger.h"
#include "network/Acceptor.h"
#include "network/Connector.h"
#include "util/MemoryStream.h"
#include "time/Timestamp.h"
#include "LuaChannel.h"
#include "LuaHttpChannel.h"

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

	lua_pushlightuserdata(L, app);
	lua_setfield(L, LUA_REGISTRYINDEX, "app");

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
        { "Read", LuaChannel::LRead },
        { "Write", LuaChannel::LWrite },
        { "Close", LuaChannel::LClose },
		{ NULL, NULL },
    };
    CreateMetaTable("metaChannel", metaChannel, LuaChannel::LRelease);

    const luaL_Reg metaHttpChannel[] = {
        { "GetUrl", LuaHttpChannel::LGetUrl },
        { "GetHeader", LuaHttpChannel::LGetHeader },
        { "GetContent", LuaHttpChannel::LGetContent },
        { "SetHeader", LuaHttpChannel::LSetReplyHeader },
        { "Reply", LuaHttpChannel::LReply },
        { "Close", LuaHttpChannel::LClose },
		{ NULL, NULL },
    };
    CreateMetaTable("metaHttpChannel", metaHttpChannel, LuaHttpChannel::LRelease);
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
	luaL_requiref(script_.state(), module, func, 1);
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

int LuaFish::Register(lua_State* L) {
	luaL_checkversion(L);

	luaL_Reg methods[] = {
		{ "Stop", LuaFish::Stop },
		{ "Log", LuaFish::Log },
		{ "Now", LuaFish::Now },
		{ "Timestamp", LuaFish::Timestamp},
		{ "Dump", LuaFish::Dump},
		{ "Pack", luaseri_pack},
		{ "UnPack", luaseri_unpack},
		{ "StartTimer", LuaFish::TimerStart},
		{ "CancelTimer", LuaFish::TimerCancel},
		{ "Listen", LuaFish::AcceptorListen},
		{ "Connect", LuaFish::ConnectorConnect},
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

int LuaFish::Stop(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	app->Stop();
	return 0;
}

int LuaFish::Log(lua_State* L) {
	const char* file = luaL_checkstring(L, 1);
	const char* log = luaL_checkstring(L, 2);
	Logger::GetSingleton()->LuaLog(file, log);
	return 0;
}

int LuaFish::Now(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushnumber(L,app->Now());
	return 1;
}

int LuaFish::Timestamp(lua_State* L) {
	uint64_t now = TimeStamp();
	lua_pushnumber(L,now);
	return 1;
}

int LuaFish::Dump(lua_State* L) {
	Timer::GetPool().Dump();
	return 0;
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

int LuaFish::AcceptorListen(lua_State* L) {
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

	const char* ip = luaL_checkstring(L, 1);
	int port = luaL_checkinteger(L, 2);
	luaL_checktype(L, 3, LUA_TFUNCTION);
	int callback = luaL_ref(L, LUA_REGISTRYINDEX);

	void* ud = lua_newuserdata(L, sizeof(Network::Acceptor));
	luaL_newmetatable(L, "metaAcceptor");
    lua_setmetatable(L, -2);

	Network::Acceptor* acceptor = new(ud) Network::Acceptor(app->Poller());

	acceptor->SetCallback(std::bind(&LuaFish::OnAccept, app->Lua(), _1, _2, _3));
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
	luaL_newmetatable(L, "metaConnector");
    lua_setmetatable(L, -2);

    Network::Connector* connector = new(ud) Network::Connector(app->Poller());

    connector->SetCallback(std::bind(&LuaFish::OnConnect, app->Lua(), _1, _2, _3));
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

	luaL_checktype(L, 3, LUA_TFUNCTION);
	luaL_checktype(L, 4, LUA_TFUNCTION);
	luaL_checktype(L, 5, LUA_TFUNCTION);

	int error = luaL_ref(L, LUA_REGISTRYINDEX);
	int close = luaL_ref(L, LUA_REGISTRYINDEX);
	int data = luaL_ref(L, LUA_REGISTRYINDEX);

	void* ud = lua_newuserdata(L, sizeof(LuaChannel));
	luaL_newmetatable(L, "metaChannel");
    lua_setmetatable(L, -2);

    lua_pushvalue(L, -1);

    int reference = luaL_ref(L, LUA_REGISTRYINDEX);

    LuaChannel* channel = new(ud) LuaChannel(app->Poller(), fd, app->Lua(), header);

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
	luaL_checktype(L, 2, LUA_TFUNCTION);
	int callback = luaL_ref(L, LUA_REGISTRYINDEX);

	void* ud = lua_newuserdata(L, sizeof( LuaHttpChannel ));
	luaL_newmetatable(L, "metaHttpChannel");
	lua_setmetatable(L, -2);

	lua_pushvalue(L, -1);
	int reference = luaL_ref(L, LUA_REGISTRYINDEX);

	LuaHttpChannel* channel = new(ud)LuaHttpChannel(app->Poller(), fd, app->Lua());

	channel->SetReference(reference);
	channel->SetCallback(callback);

	channel->EnableRead();

	return 1;
}
