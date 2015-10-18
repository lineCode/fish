#ifndef CLIENTACCEPTOR_H
#define CLIENTACCEPTOR_H

#include "network/Acceptor.h"
#include "ClientSession.h"
#include "ServerApp.h"

class ClientAcceptor : public Network::Acceptor<ClientSession>
{
public:
	typedef Network::Acceptor<ClientSession> Super;

public:
	ClientAcceptor(Network::EventPoller* poller,ServerApp* app);
	~ClientAcceptor(void);

	ClientSession* MakeSession(int fd);

private:
	ServerApp* _app;
};

#endif