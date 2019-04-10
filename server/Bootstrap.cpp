#include "Bootstrap.h"
#include "logger/Logger.h"
#include "logger/LoggerClient.h"
#include "network/EventPoller.h"
#include "network/Address.h"
#include "util/Util.h"
#include "logger/LoggerServer.h"
//#include "db/DbApp.h"
#include "agent/AgentApp.h"

#include "util/Util.h"
#include "common.h"
#include "getopt.h"
#include <assert.h>

#ifdef WIN32
#include <direct.h>
#define chdir _chdir
#endif

using namespace rapidjson;

Bootstrap::Bootstrap() {
}

Bootstrap::~Bootstrap(void) {
}

void Bootstrap::Startup(int argc, const char* argv[]) {

	int serverType = -1;
	int serverId= - 1;
	char c;
	while ((c = getopt(argc, (char*const*)argv, "c:s:i:")) != -1 ) {
		switch ( c ) {
			case 'c':
				assert(Util::LoadJson(config_, optarg) == 0);
				break;
			case 's':
				serverType = atoi(optarg);
				if ( serverType < SERVER_TYPE::eLOG || serverType >= SERVER_TYPE::eMAX ) {
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

	if ( serverType == -1 || serverId == -1 ) {
		Util::Exit(std::string("error opt"));
	}

	if (config_.HasMember("workDir")) {
		const char* workDir = config_["workDir"].GetString();
		chdir(workDir);
	}

	int hostId = config_["hostId"].GetInt();
	std::string appName = fmt::format("{}{:02}_{:04}", SERVER_TYPE_NAME[serverType], serverId, hostId);

	Util::SetProcessName(appName.c_str());

	if (!config_.HasMember("logger")) {
		Util::Exit(std::string("config logger not found"));
	}

	const rapidjson::Value& loggerCfg = config_["logger"];

	Network::EventPoller* poller = new Network::EventPoller();

	Logger* logger = NULL;
	
	if (serverType == SERVER_TYPE::eLOG) {
		const char* loggerPath = "./";
		if ( loggerCfg.HasMember("path") ) {
			loggerPath = loggerCfg["path"].GetString();
		}
		logger = new Logger(new LoggerServer(loggerPath));
	} else {
		if (!loggerCfg.HasMember("addr")) {
			Util::Exit("logger addr not found");
		}

		const rapidjson::Value& loggerAddrCfg = loggerCfg["addr"];

		if (loggerAddrCfg.HasMember("ip")) {
			const char* ip = loggerAddrCfg["ip"].GetString();
			int port = loggerAddrCfg["port"].GetInt();

			Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
			logger = new Logger(new LoggerClient(addr, poller));

		} else if (loggerAddrCfg.HasMember("ipc")) {
#ifdef WIN32
			Util::Exit("win32 not support ipc");
#else
			const char* ipc = loggerAddrCfg["ipc"].GetString();
			Network::Addr addr = Network::Addr::MakeUNIXAddr(ipc);
			logger = new Logger(new LoggerClient(addr, poller));
#endif
		} else {
			Util::Exit("logger addr error");
		}
	}

	switch (serverType) {
		case SERVER_TYPE::eLOG: {
			RunLogger(poller);
			break;
		}
		case SERVER_TYPE::eDB: {
			RunDb(poller);
			break;
		}
		case SERVER_TYPE::eLOGIN: {
			RunLogin(poller);
			break;
		}
		case SERVER_TYPE::eAGENT: {
			RunAgent(poller);
			break;
		}
		case SERVER_TYPE::eAGENT_MASTER: {
			RunAgentMaster(poller);
			break;
		}
		case SERVER_TYPE::eSCENE: {
			RunScene(poller);
			break;
		}
		case SERVER_TYPE::eSCENE_MASTER: {
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
	/*DbApp app(poller);
	app.Init(config_);
	app.Run();
	app.Fina();*/
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