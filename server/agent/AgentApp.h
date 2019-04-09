#ifndef AGENTAPP_H
#define AGENTAPP_H

#include "ServerApp.h"
#include <vector>
#include <map>
#include <string>

#include "network/Acceptor.h"
#include "network/Connector.h"
#include "network/Address.h"
#include "network/HttpChannel.h"
#include "network/ClientManager.h"
#include "util/Singleton.h"
#include "document.h" 


class AgentApp : public ServerApp, public Singleton<AgentApp> {
public:
	AgentApp(Network::EventPoller* poller);

	~AgentApp(void);

	virtual int Init(rapidjson::Document& config);

private:
	Network::ClientManager* clientMgr_;
};

#define AGENT_APP AgentApp::GetSingleton()
#endif
