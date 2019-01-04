#include "LoggerApp.h"
#include "network/EventPoller.h"
#include "util/Util.h"
#include "util/format.h"
#include "Logger.h"
#include "document.h" 

int main(int argc, char *argv[]) {
	assert(argc > 1);
 	const char* file = argv[1];

 	rapidjson::Document config;
 	assert(Util::LoadJson(config, file) == 0);

 	Network::EventPoller* poller = new Network::EventPoller();
	
	const char* loggerPath = NULL;
	if (config.HasMember("loggerPath")) {
		loggerPath = config["loggerPath"].GetString();
	} 

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
