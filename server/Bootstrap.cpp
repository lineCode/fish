#include "Bootstrap.h"
#include "Logger.h"
#include "FishApp.h"
#include "ServerApp.h"
#include "network/EventPoller.h"
#include "network/Address.h"

#include <assert.h>

using namespace rapidjson;

Bootstrap::Bootstrap(const char* file) {
	LoadConfig(file);
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

void Bootstrap::LoadConfig(const char* configFile) {
	FILE* file = fopen(configFile,"r");
	assert(file != NULL);
	fseek(file,0,SEEK_END);
	int len = ftell(file);
	char* json = (char*)malloc(len);
	memset(json,0,len);
	rewind(file);
	fread(json,1,len,file);
	fclose(file);
	assert(config_.ParseInsitu(json).HasParseError() == false);
	//free(json);
}
