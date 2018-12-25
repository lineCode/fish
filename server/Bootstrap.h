#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H
#include "reader.h"
#include "document.h"  

class Bootstrap
{
public:
	Bootstrap(void);
	~Bootstrap(void);

	void Startup(const char* file);

private:
	void LoadConfig(const char* file);

private:
	rapidjson::Document _config;
};

#endif