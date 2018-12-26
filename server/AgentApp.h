#ifndef AGENTAPP_H
#define AGENTAPP_H

#include "network/Acceptor.h"
#include "network/Address.h"
#include "ServerApp.h"

class AgentApp : public ServerApp {
public:
	AgentApp(std::string file);

	~AgentApp(void);

	virtual int Init();

	virtual int Fina();

	virtual int Run();

	virtual void HandleTimeout();

	virtual void ListenClient(std::string ip,int port);
	
	virtual void OnClientAccept(int fd, Network::Addr& addr);

private:
	std::string file_;
	Network::Acceptor* clientAcceptor_;
}




#endif