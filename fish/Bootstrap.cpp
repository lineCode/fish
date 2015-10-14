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
	
	app->Init();

	app->Run();
}

void Bootstrap::LoadConfig(const char* configFile)
{
	FILE* file = fopen(configFile,"r");
	assert(file != NULL);

	fseek(file,0,SEEK_END);
	int len = ftell(file);
	char* json = (char*)malloc(len+1);
	rewind(file);
	fread(json,1,len,file);
	json[len] = 0;

	assert(_config.ParseInsitu(json).HasParseError() == false);
}