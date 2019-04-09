#include "Bootstrap.h"
#include "logger/Logger.h"
#include "logger/LoggerClient.h"
#include "network/EventPoller.h"
#include "network/Address.h"
#include "util/Util.h"
#include "logger/LoggerServer.h"
#include "db/DbApp.h"
#include "agent/AgentApp.h"

#include "common.h"
#include "getopt.h"
#include <assert.h>

using namespace rapidjson;

Bootstrap::Bootstrap() {
}

Bootstrap::~Bootstrap(void) {
}

void Bootstrap::Startup(int argc, const char* argv[]) {

	int serverType;
	int serverId;
	char c;
	while ( ( c = getopt(argc, (char*const*)argv, "c:s:i:") ) != -1 ) {
		switch ( c ) {
			case 'c':
				assert(Util::LoadJson(config_, optarg) == 0);
				break;
			case 's':
				serverType = atoi(optarg);
				if ( serverType < eSERVER_TYPE::LOG || serverType >= eSERVER_TYPE::MAX ) {
					Util::Exit(fmt::format("unknown server type:{}", serverType));
				}
				break;
			case 'i':
				serverId = atoi(optarg);
				break;
			default: {
				Util::Exit(fmt::format("unknown opt:{}",  optarg));
			}
				
		}
	}

	Network::EventPoller* poller = new Network::EventPoller();

	Logger* logger = NULL;
	if (serverType == eSERVER_TYPE::LOG) {
		const char* loggerPath = NULL;
		if ( config_.HasMember("loggerPath") ) {
			loggerPath = config_["loggerPath"].GetString();
		}
		logger = new Logger(new LoggerServer(loggerPath));
	} else {
		if (!config_.HasMember("loggerAddr")) {
			Util::Exit("logger addr not found");
		}

		const char* ip = config_["loggerAddr"]["ip"].GetString();
		int port = config_["loggerAddr"]["port"].GetInt();

		Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
		logger = new Logger(new LoggerClient(addr, poller));
	}

	switch (serverType) {
		case eSERVER_TYPE::LOG: {
			RunLogger(poller);
			break;
		}
		case eSERVER_TYPE::DB: {
			RunDb(poller);
			break;
		}
		case eSERVER_TYPE::LOGIN: {
			RunLogin(poller);
			break;
		}
		case eSERVER_TYPE::AGENT: {
			RunAgent(poller);
			break;
		}
		case eSERVER_TYPE::AGENT_MASTER: {
			RunAgentMaster(poller);
			break;
		}
		case eSERVER_TYPE::SCENE: {
			RunScene(poller);
			break;
		}
		case eSERVER_TYPE::SCENE_MASTER: {
			RunSceneMaster(poller);
			break;
		}
		default: {
			Util::Exit(fmt::format("unknown server type:{}",  serverType));
		}
	}

	delete logger;
	delete poller;
}

void Bootstrap::RunLogger(Network::EventPoller* poller) {
	ServerApp app("logger", poller);
	app.Init("logger");
	app.Run();
	app.Fina();
}

void Bootstrap::RunDb(Network::EventPoller* poller) {
	DbApp app(poller);
	app.Init(config_);
	app.Run();
	app.Fina();
}

void Bootstrap::RunLogin(Network::EventPoller* poller) {
	ServerApp app("login", poller);
	app.Init("login");
	app.Run();
	app.Fina();
}

void Bootstrap::RunAgent(Network::EventPoller* poller) {
	AgentApp app(poller);
	app.Init(config_);
	app.Run();
	app.Fina();
}

void Bootstrap::RunAgentMaster(Network::EventPoller* poller) {
	ServerApp app("agentmaster", poller);
	app.Init("agentmaster");
	app.Run();
	app.Fina();
}

void Bootstrap::RunScene(Network::EventPoller* poller) {
	ServerApp app("scene", poller);
	app.Init("scene");
	app.Run();
	app.Fina();
}

void Bootstrap::RunSceneMaster(Network::EventPoller* poller) {
	ServerApp app("scenemaster", poller);
	app.Init("scenemaster");
	app.Run();
	app.Fina();
}