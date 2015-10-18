#include "ClientAcceptor.h"


ClientAcceptor::ClientAcceptor(Network::EventPoller* poller,ServerApp* app):Super(poller),_app(app)
{
}


ClientAcceptor::~ClientAcceptor(void)
{
}

ClientSession* ClientAcceptor::MakeSession(int fd)
{
	ClientSession* session = new ClientSession(_poller,fd);
	session->SetApp(_app);
	return session;
}