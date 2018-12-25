#ifndef SERVERACCEPTOR_H
#define SERVERACCEPTOR_H
#include "network/Acceptor.h"
#include "network/EventPoller.h"


class ServerAcceptor : public Network::Acceptor
{
public:
	typedef Network::Acceptor Super;
public:
	ServerAcceptor(ServerApp* app,Network::EventPoller* poller):_app(app),Super(poller)
	{
	}

	virtual ~ServerAcceptor()
	{
	}


private:
	ServerApp* _app;
};
#endif