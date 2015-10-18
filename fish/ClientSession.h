#ifndef CLIENTSESSION_H
#define CLIENTSESSION_H

#include "network/Session.h"

class ClientSession : public Network::Session
{
public:
	typedef Network::Session Super;

public:
	ClientSession(Network::EventPoller* poller,int fd);
	~ClientSession(void);

	int Init();

	int Fina();

	int Forward(const char * ptr,int size);

	void SetApp(ServerApp* app);

private:
	ServerApp* _app;
};

#endif