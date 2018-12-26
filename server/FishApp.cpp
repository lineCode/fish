#include "FishApp.h"
#include "Logger.h"
#include "util/format.h"
#include "network/HttpChannel.h"

FishApp::FishApp(std::string file) : file_(file)
{
	clientAcceptor_ = new Network::Acceptor(poller_);
	httpAcceptor_ = new Network::Acceptor(poller_);
}

FishApp::~FishApp(void)
{
}

int FishApp::Init()
{
	LOG_ERROR(fmt::format("FishApp start:{}", file_));
	ServerApp::Init();

	Network::Acceptor::OnConnection callback = std::bind(&FishApp::OnClientAccept, this, std::placeholders::_1, std::placeholders::_2);
	clientAcceptor_->SetCallback(callback);

	lua_->DoFile(file_);

	return 0;
}

void FishApp::ConnectMongo(const char* host,int port)
{

}

void FishApp::ListenClient(std::string ip,int port)
{
	clientAcceptor_->Listen(ip.c_str(), port);
}

void FishApp::OnClientAccept(int fd, Network::Addr& addr)
{
	std::cout << addr.ToStr() << std::endl;
}

void FishApp::OnHttpAccept(int fd, Network::Addr& addr)
{
	std::cout << addr.ToStr() << std::endl;

	Network::HttpChannel* channel = new Network::HttpChannel(poll_,fd);
	channel->EnableRead();
}


int FishApp::Fina()
{
	ServerApp::Fina();
	return 0;
}

int FishApp::Run()
{
	ServerApp::Run();
	return 0;
}

void FishApp::HandleTimeout()
{
	ServerApp::HandleTimeout();
}
