#ifndef FISHAPP_H
#define FISHAPP_H

#include "ServerApp.h"
#include <vector>
#include <map>
#include <string>

#include "network/Acceptor.h"
#include "network/Address.h"


class FishApp:public ServerApp
{
public:
	FishApp(std::string file);

	~FishApp(void);

	virtual int Init();

	virtual void ConnectMongo(const char* host,int port);

	virtual void ListenClient(std::string ip,int port);
	
	virtual void ListenHttp(std::string ip,int port);
	
	virtual void OnClientAccept(int fd, Network::Addr& addr);

	virtual void OnHttpAccept(int fd, Network::Addr& addr);

	virtual int Fina();

	virtual int Run();

	virtual void HandleTimeout(Timer* timer, void* userdata);

private:
	std::string file_;

	Network::Acceptor* clientAcceptor_;
	Network::Acceptor* httpAcceptor_;
};

#endif
