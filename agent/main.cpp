#include "network/EventPoller.h"
#include "util/Util.h"
#include "util/format.h"
#include "logger/Logger.h"
#include "logger/LoggerClient.h"
#include "document.h" 
#include "AgentApp.h"

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

	if ( !config.HasMember("loggerAddr") ) {
		Util::Exit("logger addr not found");
	}

	const char* ip = config["loggerAddr"]["ip"].GetString();
	int port = config["loggerAddr"]["port"].GetInt();

	Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
	Logger* logger = new Logger(new LoggerClient(addr, poller));

	{
		AgentApp app(poller);
		app.Init(config);
		app.Run();
		app.Fina();
	}

	delete logger;
	delete poller;

	return 0;
}
