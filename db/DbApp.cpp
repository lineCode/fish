#include "DbApp.h"
#include "logger/Logger.h"
#include "DbTask.h"

template <>
DbApp* Singleton<DbApp>::singleton_ = 0;

DbApp::DbApp(Network::EventPoller* poller) : ServerApp(poller) {
	dbThreadPool_ = NULL;
}

DbApp::~DbApp(void) {
}

int DbApp::Init(rapidjson::Document& config) {
	LOG_ERROR(fmt::format("DbApp start"));

	lua_->Require("db", DbApp::Register);

	dbThreadPool_ = new DbThreadPool();

	int dbThreadCount = 4;
	if (config.HasMember("dbThreadCount")) {
		dbThreadCount = config["dbThreadCount"].GetInt();
	}

	const char* dbIp = "127.0.0.1";
	int dbPort = 3306;
	const char* dbUser = "root";
	const char* dbPwd = "123456";
	const char* dbName = "mrq";
	if (config.HasMember("db")) {
		if (config["db"].HasMember("ip")) {
			dbIp = config["db"]["ip"].GetString();
		}
	
		if (config["db"].HasMember("port")) {
			dbPort = config["db"]["port"].GetInt();
		}
		if (config["db"].HasMember("user")) {
			dbUser = config["db"]["user"].GetString();
		}
		if (config["db"].HasMember("pwd")) {
			dbPwd = config["db"]["pwd"].GetString();
		}	
		if (config["db"].HasMember("name")) {
			dbName = config["db"]["name"].GetString();
		}
	}

	dbThreadPool_->Init(dbThreadCount, dbIp, dbPort, dbUser, dbPwd, dbName);

	std::string boot("db");
	ServerApp::Init(boot);
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
	APP->GetThreadPool()->PostTask(task);

	return 0;
}

int DbApp::LExecute(lua_State* L) {
	size_t size;
	const char* sql = luaL_checklstring(L, 1, &size);
	luaL_checktype(L, 2, LUA_TFUNCTION);
	int reference = luaL_ref(L, LUA_REGISTRYINDEX);

	DbTask* task = new DbRawSqlTask(reference, sql, size);
	APP->GetThreadPool()->PostTask(task);

	return 0;
}
