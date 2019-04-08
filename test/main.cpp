#include <unordered_map>
#include <iostream>
#include "oolua.h"
#include "ServerApp.h"
#include "logger/Logger.h"
#include "logger/LoggerClient.h"

#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif


int main(int argc,const char* argv[]) {
#if defined(_WIN32)
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
#endif

	Network::EventPoller* poller = new Network::EventPoller();
	Network::Addr addr = Network::Addr::MakeIP4Addr("127.0.0.1", 1989);
	Logger* logger = new Logger(new LoggerClient(addr, poller));

	
	ServerApp app(poller);
	app.Init("test");
	app.Run();
	app.Fina();
	return 0;
}
