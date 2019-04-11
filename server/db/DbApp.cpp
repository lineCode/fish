#include "DbApp.h"
#include "logger/Logger.h"
#include "DbTask.h"

template <>
DbApp* Singleton<DbApp>::singleton_ = 0;

DbApp::DbApp(Network::EventPoller* poller) : ServerApp("db", poller) {
	dbThreadPool_ = NULL;
}

DbApp::~DbApp(void) {
}

int DbApp::Init(const rapidjson::Value& config) {
	LOG_ERROR(fmt::format("DbApp start"));

	lua_->Require("db", DbApp::Register);

	dbThreadPool_ = new DbThreadPool();

	int threadCount = 4;
	int port = 3306;
	const char* ip = "127.0.0.1";
	const char* user = "root";
	const char* pwd = "12345678";
	const char* name = "mrq";

	if (config.HasMember("db")) {
		if (config["db"].HasMember("threadCount")) {
			threadCount = config["db"]["threadCount"].GetInt();
		}
		if (config["db"].HasMember("ip")) {
			ip = config["db"]["ip"].GetString();
		}
		if (config["db"].HasMember("port")) {
			port = config["db"]["port"].GetInt();
		}
		if (config["db"].HasMember("user")) {
			user = config["db"]["user"].GetString();
		}
		if (config["db"].HasMember("pwd")) {
			pwd = config["db"]["pwd"].GetString();
		}	
		if (config["db"].HasMember("name")) {
			name = config["db"]["name"].GetString();
		}
	}

	dbThreadPool_->Init(threadCount, ip, port, user, pwd, name);

	ServerApp::Init("db");
	return 0;
}

int DbApp::Fina() {
	delete dbThreadPool_;
	dbThreadPool_ = NULL;
	return 0;
}

DbThreadPool* DbApp::GetThreadPool() {
	return dbThreadPool_;
}

int DbApp::Register(lua_State* L) {
	luaL_checkversion(L);

	luaL_Reg methods[] = {
		{ "Query", DbApp::LQuery },
		{ "Execute", DbApp::LExecute },
		{ NULL, NULL },
	};

	luaL_newlibtable(L, methods);
	luaL_setfuncs(L, methods, 0);

	return 1;
}

int DbApp::LQuery(lua_State* L) {
	size_t size;
	const char* sql = luaL_checklstring(L, 1, &size);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	int reference = luaL_ref(L, LUA_REGISTRYINDEX);

	DbTask* task = new DbQueryTask(reference, sql, size);
	DB_APP->GetThreadPool()->PostTask(task);

	return 0;
}

int DbApp::LExecute(lua_State* L) {
	size_t size;
	const char* sql = luaL_checklstring(L, 1, &size);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	int reference = luaL_ref(L, LUA_REGISTRYINDEX);

	DbTask* task = new DbRawSqlTask(reference, sql, size);
	DB_APP->GetThreadPool()->PostTask(task);

	return 0;
}
