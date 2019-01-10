#include "LoggerApp.h"
#include "logger/Logger.h"

LoggerApp::LoggerApp(Network::EventPoller* poller) : ServerApp(poller) {

}

LoggerApp::~LoggerApp(void) {
}

int LoggerApp::Init() {
	LOG_ERROR(fmt::format("LoggerApp start"));
	std::string boot("logger");
	ServerApp::Init(boot);
	return 0;
}

