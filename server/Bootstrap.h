#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H
#include "reader.h"
#include "document.h"  

class Bootstrap
{
public:
	Bootstrap(const char* file);
	~Bootstrap(void);

	void Startup();

private:
	rapidjson::Document config_;
};

#endif