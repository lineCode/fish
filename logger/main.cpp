#include "LoggerApp.h"
#include "network/EventPoller.h"
#include "util/Util.h"
#include "util/format.h"
#include "document.h" 

int main(int argc, char *argv[]) {
	assert(argc > 1);
 	const char* file = argv[1];

 	rapidjson::Document config;
 	assert(Util::LoadJson(config, file) == 0);

 	Network::EventPoller* poller = new Network::EventPoller();

	if (!config.HasMember("loggerPath")) {
		std::cerr << "config not define loggerPath" << std::endl;
		assert(0);
	}

	const char* loggerPath = config["loggerPath"].GetString();

	Logger* logger = new Logger(loggerPath);

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
