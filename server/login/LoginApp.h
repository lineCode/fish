#ifndef LOGINAPP_H
#define LOGINAPP_H

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


class LoginApp : public ServerApp, public Singleton<LoginApp> {
public:
	LoginApp(Network::EventPoller* poller);

	~LoginApp(void);

	virtual int Init(rapidjson::Document& config);

private:
	Network::ClientManager* clientMgr_;
};

#define LOGIN_APP LoginApp::GetSingleton()
#endif
