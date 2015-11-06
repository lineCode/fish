#include "Bootstrap.h"

#include "FishApp.h"
#include "ServerApp.h"

#include <assert.h>
#include <iostream>

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

	FishApp* app = new FishApp(_config["boot"].GetString());

	if (_config.HasMember("mongo"))
		app->ConnectMongo(_config["mongo"]["ip"].GetString(),_config["mongo"]["port"].GetInt());

	if (_config.HasMember("client"))
		app->ListenClient(_config["client"]["ip"].GetString(),_config["client"]["port"].GetInt());
	
	app->Init();

	app->Run();

	app->Fina();
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

	assert(_config.ParseInsitu(json).HasParseError() == false);
}