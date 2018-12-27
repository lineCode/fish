#include "Bootstrap.h"
#include <assert.h>
#ifdef _WIN32
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif
#include "util/PoolObject.h"

class Test : public PoolObject<Test> {
public:
	int a;
	int b;
	double c;

	Test() {

	}
	~Test() {

	}
};

int main(int argc, char *argv[])
{
// #if defined(_WIN32)
// 	WSADATA wsa_data;
// 	WSAStartup(0x0201, &wsa_data);
// #endif
// 	assert(argc > 1);
// 	const char* config = argv[1];

// 	Bootstrap bootstrap;
// 	bootstrap.Startup(config);

	Test* test = new Test();

	return 0;
}