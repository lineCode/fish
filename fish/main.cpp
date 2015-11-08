#include "Bootstrap.h"
#include <assert.h>

int main(int argc, char *argv[])
{
	assert(argc > 1);
	const char* config = argv[1];

	Bootstrap bootstrap;
	bootstrap.Startup(config);

	return 0;
}