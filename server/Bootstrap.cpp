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

	const char* ip = config_["loggerAddr"]["ip"].GetString();
	int port = config_["loggerAddr"]["port"].GetInt();

	Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
	LoggerInterface* loggerInterface = new LoggerClient(addr, poller);
	
	Logger* logger = new Logger(loggerInterface);

	ClientManager* clientMgr = NULL;
	if ( config_.HasMember("clientAddr") ) {
		const char* clientIp = config_["clientAddr"]["ip"].GetString();
		int clientPort = config_["clientAddr"]["port"].GetInt();

		int maxClient = 1000;
		if (config_.HasMember("maxClient")) {
			maxClient = config_["maxClient"].GetInt();
		}
		clientMgr = new ClientManager(maxClient, 1);
		Network::Addr addr = Network::Addr::MakeIP4Addr(clientIp, clientPort);
		if ( clientMgr->Listen(addr) < 0 ) {
			std::cerr << fmt::format("client manager listen:{} error", addr.ToStr()) << std::endl;
			return;
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
	if ( clientMgr ) {
		delete clientMgr;
	}
	delete poller;
}
