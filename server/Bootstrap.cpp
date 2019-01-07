#include "Bootstrap.h"
#include "logger/Logger.h"
#include "logger/LoggerClient.h"
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

	const char* ip = config_["loggerAddr"]["ip"].GetString();
	int port = config_["loggerAddr"]["port"].GetInt();

	Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
	LoggerInterface* loggerInterface = new LoggerClient(addr, poller);
	
	Logger* logger = new Logger(loggerInterface);
	
	const char* file = config_["boot"].GetString();
	std::string boot(file);
	{
		FishApp app(poller);
		app.Init(boot);
		app.Run();
		app.Fina();
	}
	
	delete logger;
	delete poller;
}
