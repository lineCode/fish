#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H
#include "reader.h"
#include "document.h"  
#include "network/EventPoller.h"

class Bootstrap
{
public:
	Bootstrap();
	~Bootstrap(void);

	void Startup(int argc, const char* argv[]);
private:
	rapidjson::Document config_;
};

#endif