#include "Bootstrap.h"
#include "logger/Logger.h"
#include "logger/LoggerClient.h"
#include "FishApp.h"
#include "network/EventPoller.h"
#include "network/Address.h"
#include "util/Util.h"

#include <assert.h>

using namespace rapidjson;

Bootstrap::Bootstrap(const char* file) {
	assert(Util::LoadJson(config_, file) == 0);
}


Bootstrap::~Bootstrap(void) {
}

void Bootstrap::Startup() {

	Network::EventPoller* poller = new Network::EventPoller();

	if (!config_.HasMember("loggerAddr")) {
		Util::Exit("logger addr not found");
	}

	const char* ip = config_["loggerAddr"]["ip"].GetString();
	int port = config_["loggerAddr"]["port"].GetInt();

	Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
	Logger* logger = new Logger(new LoggerClient(addr, poller));

	const char* file = config_["boot"].GetString();

	{
		FishApp app(poller);
		app.Init(file);
		app.Run();
		app.Fina();
	}
	
	delete logger;
	delete poller;
}
