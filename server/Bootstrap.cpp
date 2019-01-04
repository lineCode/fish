#include "Bootstrap.h"
#include "Logger.h"
#include "FishApp.h"
#include "ServerApp.h"
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

	Logger* logger = NULL;
	if (config_.HasMember("logPath")) {
		logger = new Logger(config_["logPath"].GetString());
	} else {
		const char* ip = config_["logPath"]["ip"].GetString();
		int port = config_["logPath"]["port"].GetInt();
		Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
		logger = new Logger(addr, poller);
	}
	
	{
		FishApp app(poller);
		app.Init();
		app.Run();
		app.Fina();
	}
	
	delete logger;
	delete poller;
}
