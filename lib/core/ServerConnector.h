#ifndef SERVERCONNECTOR_H
#define SERVERCONNECTOR_H

#include "network/Connector.h"
#include "network/EventPoller.h"

template<class SESSION>
class ServerConnector : public Network::Connector<SESSION>
{
public:
	typedef Network::Connector<SESSION> Super;
public:
	ServerConnector(ServerApp* app,Network::EventPoller* poller):_app(app),Super(poller)
	{
	}

	virtual ~ServerConnector()
	{
	}

	virtual SESSION* MakeSession(int fd)
	{
		SESSION* session = new SESSION(_poller,fd);
		session->SetApp(_app);
		return session;
	}

private:
	ServerApp* _app;
};

#endif