#include "FishApp.h"
#include "Logger.h"
#include "util/format.h"

FishApp::FishApp(std::string file):
	_file(file),
	_mongoConnector(this,_poller),
	_clientAcceptor(this,_poller)
{
}

FishApp::~FishApp(void)
{
}

int FishApp::Init()
{
	LOG_ERROR(fmt::format("FishApp start:{}",_file));
	ServerApp::Init();

	_LuaManager->DoFile(_file.c_str());

	return 0;
}

void FishApp::ConnectMongo(const char* host,int port)
{
	if (_mongoConnector.Connect("192.168.15.246",10005) < 0)
	{
		LOG_ERROR(fmt::format("FishApp connect mongodb fail"));
		assert(false);
	}

	LOG_ERROR(fmt::format("FishApp connect mongodb success"));
	this->Mongo(_mongoConnector.GetSession());
}

void FishApp::ListenClient(const char* host,int port)
{
	_clientAcceptor.Listen(host,port);
}

int FishApp::Fina()
{
	return 0;
}

int FishApp::Run()
{
	ServerApp::Run();
	return 0;
}

int FishApp::HandleTimeout()
{
	ServerApp::HandleTimeout();
	return 0;
}