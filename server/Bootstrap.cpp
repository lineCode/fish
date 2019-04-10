#include "Bootstrap.h"
#include "logger/Logger.h"
#include "logger/LoggerClient.h"
#include "network/EventPoller.h"
#include "network/Address.h"
#include "util/Util.h"
#include "logger/LoggerServer.h"
//#include "db/DbApp.h"
#include "agent/AgentApp.h"
#include "login/LoginApp.h"
#include "util/Util.h"
#include "Common.h"
#include <assert.h>

#ifdef WIN32
#include "getopt.h"
#include <direct.h>
#define chdir _chdir
#else
#include <unistd.h>
#endif

using namespace rapidjson;

Bootstrap::Bootstrap() {
}

Bootstrap::~Bootstrap(void) {
}

void Bootstrap::Startup(int argc, const char* argv[]) {
	
	int appType = -1;
	int appId = - 1;
	int appUid = -1;
	char c;
	while ((c = getopt(argc, (char*const*)argv, "c:s:i:")) != -1 ) {
		switch (c) {
			case 'c':
				if ( Util::LoadJson(config_, optarg) < 0 ) {
					Util::Exit("parse config error");
				}
				break;
			case 's':
				appType = atoi(optarg);
				if (appType < APP_TYPE::eLOG || appType >= APP_TYPE::eMAX) {
					Util::Exit(fmt::format("unknown server type:{}", appType));
				}
				break;
			case 'i':
				appId = atoi(optarg);
				break;
			default: {
				Util::Exit(fmt::format("unknown opt:{}",  c));
			}
		}
	}

	if ( appType == -1 || appId == -1 ) {
		Util::Exit(std::string("error opt"));
	}

	appUid = appId * 100 + appType;

	if (config_.HasMember("workDir")) {
		const char* workDir = config_["workDir"].GetString();
		chdir(workDir);
	}

	int hostId = config_["hostId"].GetInt();
	std::string serverTypeName = SERVER_TYPE_NAME[appType];

	std::string appName = fmt::format("{}{:02}_{:04}", serverTypeName, appId, hostId);

	Util::SetProcessName(appName.c_str());

	if (!config_.HasMember("logger")) {
		Util::Exit(std::string("config logger not found"));
	}

	const rapidjson::Value& loggerCfg = config_["logger"];

	Network::EventPoller* poller = new Network::EventPoller();

	Logger* logger = NULL;
	
	if (appType == APP_TYPE::eLOG) {
		const char* path = "./";
		if (loggerCfg.HasMember("path")) {
			path = loggerCfg["path"].GetString();
		}
		logger = new Logger(new LoggerServer(path));
	} else {
		if (!loggerCfg.HasMember("addr")) {
			Util::Exit("logger addr not found");
		}

		const rapidjson::Value& addrCfg = loggerCfg["addr"];

		if (addrCfg.HasMember("ip")) {
			const char* ip = addrCfg["ip"].GetString();
			int port = addrCfg["port"].GetInt();

			Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
			logger = new Logger(new LoggerClient(addr, poller));

		} else if (addrCfg.HasMember("ipc")) {
#ifdef WIN32
			Util::Exit("win32 not support ipc");
#else
			const char* ipc = addrCfg["ipc"].GetString();
			Network::Addr addr = Network::Addr::MakeUNIXAddr(ipc);
			logger = new Logger(new LoggerClient(addr, poller));
#endif
		} else {
			Util::Exit("logger addr error");
		}
	}

	LOG_INFO(fmt::format("starting server:{} ,app type:{}, app id:{}, app uid:{}, host id:{}", appName, appType, appId, appUid, hostId));

	ServerApp* app = NULL;

	switch ( appType ) {
		case APP_TYPE::eLOG: {
			app = new ServerApp("logger", poller);
			app->Init("logger");
			break;
		}
		case APP_TYPE::eDB: {
			// app = new DbApp(poller);
			// app->Init(config_);
			break;
		}
		case APP_TYPE::eLOGIN: {
			LoginApp* loginApp = new LoginApp(poller);
			if (!config_.HasMember("loginClient")) {
				Util::Exit("config loginClient not found");
			}
			loginApp->Init(config_["loginClient"]);
			app = loginApp;
			break;
		}
		case APP_TYPE::eAGENT: {
			AgentApp* agentApp = new AgentApp(poller);
			if (!config_.HasMember("agentClient")) {
				Util::Exit("config agentClient not found");
			}
			agentApp->Init(config_["agentClient"]);
			app = agentApp;
			break;
		}
		case APP_TYPE::eAGENT_MASTER: {
			app = new ServerApp("agentmaster", poller);
			app->Init("agentmaster");
			break;
		}
		case APP_TYPE::eSCENE: {
			app = new ServerApp("scene", poller);
			app->Init("scene");
			break;
		}
		case APP_TYPE::eSCENE_MASTER: {
			app = new ServerApp("scenemaster", poller);
			app->Init("scenemaster");
			break;
		}
		default: {
			Util::Exit(fmt::format("unknown app type:{}",  appType));
		}
	}

	LuaFish* lua = app->Lua();
	lua->SetEnv("appId", appId);
	lua->SetEnv("appUid", appUid);
	lua->SetEnv("appType", appType);
	lua->SetEnv("appName", appName.c_str());

	app->Run();
	app->Fina();

	delete app;
	delete logger;
	delete poller;
}
