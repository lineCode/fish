#ifndef SERVERACCEPTOR_H
#define SERVERACCEPTOR_H
#include "network/Acceptor.h"
#include "network/EventPoller.h"

template<class SESSION>
class ServerAcceptor : public Network::Acceptor<SESSION>
{
public:
	typedef Network::Acceptor<SESSION> Super;
public:
	ServerAcceptor(ServerApp* app,Network::EventPoller* poller):_app(app),Super(poller)
	{
	}

	virtual ~ServerAcceptor()
	{
	}

	virtual SESSION* MakeSession(int fd)
	{
		SESSION* session = new SESSION(this->_poller,fd);
		session->SetApp(_app);
		return session;
	}

private:
	ServerApp* _app;
};
#endif