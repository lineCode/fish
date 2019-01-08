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

	virtual int Init(std::string& boot);

	virtual int Fina();

	virtual int Run();

	virtual void OnUpate(Timer* timer, void* userdata);

	virtual void ListenClient(std::string ip,int port);
	
	virtual void ListenHttp(std::string ip,int port);
	
	virtual void OnClientAccept(int fd, Network::Addr& addr);

	virtual void OnHttpAccept(int fd, Network::Addr& addr);

	virtual void OnHttpComplete(Network::HttpChannel* channel, void* userdata);

private:
	Network::Acceptor* clientAcceptor_;
	Network::Acceptor* httpAcceptor_;
	Network::Connector* connector_;

};

#endif
