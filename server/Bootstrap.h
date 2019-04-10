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

	void RunLogger(Network::EventPoller* poller);

	void RunDb(Network::EventPoller* poller);

	void RunLogin(Network::EventPoller* poller);

	void RunAgent(Network::EventPoller* poller);

	void RunAgentMaster(Network::EventPoller* poller);

	void RunScene(Network::EventPoller* poller);

	void RunSceneMaster(Network::EventPoller* poller);

private:
	rapidjson::Document config_;
};

#endif