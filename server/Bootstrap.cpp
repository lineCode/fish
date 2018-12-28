#include "Bootstrap.h"
#include "Logger.h"
#include "FishApp.h"
#include "ServerApp.h"

#include <assert.h>

using namespace rapidjson;

Bootstrap::Bootstrap(void)
{
}


Bootstrap::~Bootstrap(void)
{
}

void Bootstrap::Startup(const char* file)
{
	LoadConfig(file);

	const char* path = NULL;
	if (config_.HasMember("log"))
		path = config_["log"].GetString();
	Logger* logger = new Logger(path);

	FishApp* app = new FishApp(config_["boot"].GetString());

	app->ListenClient(std::string("0.0.0.0"), 1989);

	app->ListenHttp(std::string("0.0.0.0"), 1994);
	
	app->Init();

	app->Run();

	app->Fina();

	delete app;

	delete logger;
}

void Bootstrap::LoadConfig(const char* configFile)
{
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
