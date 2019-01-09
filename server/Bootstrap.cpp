#include "Bootstrap.h"
#include "logger/Logger.h"
#include "logger/LoggerClient.h"
#include "FishApp.h"
#include "ClientManager.h"
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
	LoggerInterface* loggerInterface = new LoggerClient(addr, poller);
	
	Logger* logger = new Logger(loggerInterface);

	ClientManager* clientMgr = NULL;
	if (config_.HasMember("clientAddr")) {

		int maxClient = 1000;
		if (config_.HasMember("maxClient")) {
			maxClient = config_["maxClient"].GetInt();
		}

		clientMgr = new ClientManager(maxClient, 1);

		if (config_.HasMember("maxFreq")) {
			clientMgr->SetMaxFreq(config_["maxFreq"].GetInt());
		}

		if (config_.HasMember("maxAlive")) {
			clientMgr->SetMaxAlive(config_["maxAlive"].GetInt());
		}

		if (config_.HasMember("warnFlow")) {
			clientMgr->SetWarnFlow(config_["warnFlow"].GetInt());
		}

		ip = config_["clientAddr"]["ip"].GetString();
		port = config_["clientAddr"]["port"].GetInt();

		Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
		if (clientMgr->Start(addr) < 0) {
			Util::Exit(fmt::format("client manager start:{} error", addr.ToStr()));
		}
	}
	
	const char* file = config_["boot"].GetString();
	std::string boot(file);
	{
		FishApp app(poller);
		app.Init(boot);
		app.Run();
		app.Fina();
	}
	
	delete logger;
	if (clientMgr) {
		delete clientMgr;
	}
	delete poller;
}
