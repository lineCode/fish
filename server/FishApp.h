#ifndef FISHAPP_H
#define FISHAPP_H

#include "ServerApp.h"
#include <vector>
#include <map>
#include <string>

#include "network/Acceptor.h"



class FishApp:public ServerApp
{
public:
	FishApp(std::string file);

	~FishApp(void);

	virtual int Init();

	virtual void ConnectMongo(const char* host,int port);

	virtual void ListenClient(std::string ip,int port);

	virtual void OnClientAccept(Network::Acceptor* acceptor, int fd, const char* ip, int port);

	virtual int Fina();

	virtual int Run();

	virtual void HandleTimeout();

private:
	std::string file_;

	Network::Acceptor* clientAcceptor_;
};

#endif
