#include "Bootstrap.h"
#include <assert.h>
#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

int main(int argc, const char *argv[])
{
 #if defined(_WIN32)
 	WSADATA wsa_data;
 	WSAStartup(0x0201, &wsa_data);
 #endif

 	Bootstrap bootstrap;
 	bootstrap.Startup(argc, argv);

	return 0;
}
