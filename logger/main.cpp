#include "LoggerApp.h"
#include "network/EventPoller.h"
#include "util/Util.h"
#include "util/format.h"
#include "logger/Logger.h"
#include "logger/LoggerHost.h"
#include "document.h" 

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

int main(int argc, char *argv[]) {
	assert(argc > 1);
 	const char* file = argv[1];
#if defined(_WIN32)
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

 	rapidjson::Document config;
 	assert(Util::LoadJson(config, file) == 0);

 	Network::EventPoller* poller = new Network::EventPoller();
	
	const char* loggerPath = NULL;
	if (config.HasMember("loggerPath")) {
		loggerPath = config["loggerPath"].GetString();
	} 

	Logger* logger = new Logger(new LoggerHost(loggerPath));

	{
		LoggerApp app(poller);
		app.Init();
		app.Run();
		app.Fina();
	}
	
	delete logger;
	delete poller;

	return 0;
}
