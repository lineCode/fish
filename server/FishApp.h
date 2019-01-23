#ifndef FISHAPP_H
#define FISHAPP_H

#include "ServerApp.h"
#include <vector>
#include <map>
#include <string>

#include "network/Acceptor.h"
#include "network/Connector.h"
#include "network/Address.h"
#include "network/HttpChannel.h"
#include "util/Singleton.h"


class FishApp : public ServerApp, public Singleton<FishApp> {
public:
	FishApp(Network::EventPoller* poller);

	~FishApp(void);

	virtual int Init(std::string boot);

	virtual int Fina();

	virtual int Run();

	virtual void OnUpate(Timer* timer, void* userdata);
};

#define APP FishApp::GetSingleton()
#endif
