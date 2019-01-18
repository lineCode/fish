#include "DbApp.h"
#include "logger/Logger.h"

DbApp::DbApp(Network::EventPoller* poller) : ServerApp(poller) {
	dbThreadPool_ = NULL;
}

DbApp::~DbApp(void) {
}

int DbApp::Init(rapidjson::Document& config) {
	LOG_ERROR(fmt::format("DbApp start"));
	std::string boot("db");
	ServerApp::Init(boot);

	dbThreadPool_ = new DbThreadPool();

	int dbThreadCount = 4;
	if (config.HasMember("dbThreadCount")) {
		dbThreadCount = config["dbThreadCount"].GetInt();
	}

	const char* dbIp = "127.0.0.1";
	if (config.HasMember("dbIp")) {
		dbIp = config["dbIp"].GetString();
	}

	int dbPort = 3306;
	if (config.HasMember("dbPort")) {
		dbPort = config["dbPort"].GetInt();
	}

	const char* dbUser = "root";
	if (config.HasMember("dbPort")) {
		dbUser = config["dbUser"].GetString();
	}

	const char* dbPwd = "123456";
	if (config.HasMember("dbPwd")) {
		dbPwd = config["dbPwd"].GetString();
	}

	dbThreadPool_->Init(dbThreadCount, dbIp, dbPort, dbUser, dbPwd);
	return 0;
}


int DbApp::Fina() {
	delete dbThreadPool_;
	dbThreadPool_ = NULL;
}