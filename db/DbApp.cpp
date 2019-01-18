#include "DbApp.h"
#include "logger/Logger.h"

DbApp::DbApp(Network::EventPoller* poller) : ServerApp(poller) {
	dbThreadPool_ = NULL;
}

DbApp::~DbApp(void) {
}

int DbApp::Init() {
	LOG_ERROR(fmt::format("DbApp start"));
	std::string boot("db");
	ServerApp::Init(boot);

	dbThreadPool_ = new DbThreadPool();
	dbThreadPool_->Init(4, "127.0.0.1", 3306, "root", "123456");
	return 0;
}


int DbApp::Fina() {
	delete dbThreadPool_;
	dbThreadPool_ = NULL;
}