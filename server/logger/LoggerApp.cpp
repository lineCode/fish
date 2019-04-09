#include "LoggerApp.h"
#include "logger/Logger.h"

LoggerApp::LoggerApp(Network::EventPoller* poller) : ServerApp("logger", poller) {

}

LoggerApp::~LoggerApp(void) {
}

int LoggerApp::Init() {
	LOG_ERROR(fmt::format("LoggerApp start"));
	ServerApp::Init("logger");
	return 0;
}

