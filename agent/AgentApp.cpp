#include "AgentApp.h"
#include "logger/Logger.h"
#include "util/format.h"
#include "util/Util.h"
#include "network/HttpChannel.h"



template <>
AgentApp * Singleton<AgentApp>::singleton_ = 0;

AgentApp::AgentApp(Network::EventPoller* poller) : ServerApp(poller) {
}

AgentApp::~AgentApp(void) {
	delete clientMgr_;
}

int AgentApp::Init(rapidjson::Document& config) {
	LOG_ERROR(fmt::format("AgentApp start"));

	std::string boot("agent");
	ServerApp::Init(boot);

	if (!config.HasMember("clientAddr")) {
		Util::Exit("no client addr");
	}

	int maxClient = 1000;
	if (config.HasMember("maxClient")) {
		maxClient = config["maxClient"].GetInt();
	}

	clientMgr_ = new ClientManager(maxClient, 1);

	if (config.HasMember("maxFreq")) {
		clientMgr_->SetMaxFreq(config["maxFreq"].GetInt());
	}

	if (config.HasMember("maxAlive")) {
		clientMgr_->SetMaxAlive(config["maxAlive"].GetInt());
	}

	if (config.HasMember("warnFlow")) {
		clientMgr_->SetWarnFlow(config["warnFlow"].GetInt());
	}

	const char* ip = config["clientAddr"]["ip"].GetString();
	int port = config["clientAddr"]["port"].GetInt();

	Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
	if (clientMgr_->Start(addr) < 0) {
		Util::Exit(fmt::format("client manager start:{} error", addr.ToStr()));
	}

	lua_->Require("agent", AgentApp::Register);

	return 0;
}


int AgentApp::Register(lua_State* L) {
	luaL_checkversion(L);

	luaL_Reg methods[] = {
		{ "Stop", AgentApp::LStop },
		{ "Send", AgentApp::LSendClient },
		{ "Broadcast", AgentApp::LBroadcastClient },
		{ "Close", AgentApp::LCloseClient },
		{ NULL, NULL },
	};

	luaL_newlibtable(L, methods);
	luaL_setfuncs(L, methods, 0);

	return 1;
}

int AgentApp::LStop(lua_State* L) {
	CLIENT_MGR->Stop();
	return 0;
}

char* GetBuffer(lua_State* L, int index, size_t* size) {
	char* data = NULL;
	int vt = lua_type(L, index);
	switch(vt) {
		case LUA_TSTRING: {
			const char* str = lua_tolstring(L, index, size);
			data = (char*)malloc(*size);
			memcpy(data, str, *size);
			break;
		}
		case LUA_TLIGHTUSERDATA: {
			data = (char*)lua_touserdata(L, index);
			*size = luaL_checkinteger(L, index+1);
			break;	
		}
		default: {
			luaL_error(L, "client manager get buffer error:unknow lua type:%s", lua_typename(L,vt));
		}
	}

	if (*size <= 0) {
		return NULL;
	}

	return data;
}

int AgentApp::LSendClient(lua_State* L) {
	int vid = luaL_checkinteger(L, 1);

	size_t size;
	char* data = GetBuffer(L, 2, &size);

	CLIENT_MGR->SendClient(vid, data, size);
	return 0;
}

int AgentApp::LBroadcastClient(lua_State* L) {
	luaL_checktype(L, 1, LUA_TTABLE);

	size_t size;
	char* data = GetBuffer(L, 2, &size);

	std::vector<int> vids;
	lua_pushnil(L);
	while (lua_next(L, 1) != 0) {
		int vid = luaL_checkinteger(L, -1);
		vids.push_back(vid);
		lua_pop(L, 1);
	}

	CLIENT_MGR->BroadcastClient(vids, data, size);

	return 0;
}

int AgentApp::LCloseClient(lua_State* L) {
	int vid = luaL_checkinteger(L, 1);
	CLIENT_MGR->CloseClient(vid);
	return 0;
}

