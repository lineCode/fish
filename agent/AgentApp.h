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
#include "util/Singleton.h"
#include "ClientManager.h"
#include "document.h" 


class AgentApp : public ServerApp, public Singleton<AgentApp> {
public:
	AgentApp(Network::EventPoller* poller);

	~AgentApp(void);

	virtual int Init(rapidjson::Document& config);

	virtual int Fina();

	virtual int Run();

	virtual void OnUpate(Timer* timer, void* userdata);

private:
	ClientManager* clientMgr_;
};

#define APP AgentApp::GetSingleton()
#endif
