
#include "Logger.h"
#include "Bootstrap.h"
#include <assert.h>

int main(int argc, char *argv[])
{
	assert(argc > 1);
	const char* config = argv[1];

	Logger* logger = Logger::CreateLogger();

	Bootstrap bootstrap;
	bootstrap.Startup(config);

	return 0;
}