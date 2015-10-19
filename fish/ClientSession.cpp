#include "ClientSession.h"
#include "MessageHandler.h"
#include "network/TcpReader.h"

ClientSession::ClientSession(Network::EventPoller* poller,int fd):Super(poller,fd)
{
}

ClientSession::~ClientSession(void)
{
}

int ClientSession::Init()
{
	_reader = new Network::TcpReader(this,2);
	return 0;
}

int ClientSession::Fina()
{
	return 0;
}

int ClientSession::Forward(const char * ptr,int size)
{
	uint8 method[2];
	method[0] = ptr[0];
	method[1] = ptr[1];

	MessageHelper::MessageReader reader((char*)ptr+2,size);
	DispatchHandler((method[0] << 8)|method[1],this,reader);
	free((void*)ptr);
	return 0;
}

void ClientSession::SetApp(ServerApp* app)
{
	_app = app;
}

ServerApp* ClientSession::GetApp()
{
	return _app;
}