#include "LoggerApp.h"


LoggerApp::LoggerApp(Network::EventPoller* poller) : ServerApp(poller) {

}

LoggerApp::~LoggerApp(void) {
}

int LoggerApp::Init() {
	LOG_ERROR(fmt::format("LoggerApp start"));
	ServerApp::Init();
	lua_->DoFile("logger.lua");

	OOLUA::Script& script = lua_->GetScript();
	if (!script.call("LoggerInit")) {
		LOG_ERROR(fmt::format("LoggerInit error:{}",OOLUA::get_last_error(script)));
		return -1;
	}
	return 0;
}

