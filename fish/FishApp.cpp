﻿#include "FishApp.h"
#include "Logger.h"
#include "util/format.h"

FishApp::FishApp(std::string file) : file_(file)
{
	clientAcceptor_ = new Network::Acceptor(poller_);
}

FishApp::~FishApp(void)
{
}

int FishApp::Init()
{
	LOG_ERROR(fmt::format("FishApp start:{}", file_));
	ServerApp::Init();

	Network::Acceptor::OnConnection callback = std::bind(&FishApp::OnClientAccept, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	clientAcceptor_->SetCallback(callback);

	lua_->DoFile(file_);

	return 0;
}

void FishApp::ConnectMongo(const char* host,int port)
{

}

void FishApp::ListenClient(std::string& ip,int port)
{
	clientAcceptor_->Listen(ip.c_str(), port);
}

void FishApp::OnClientAccept(Network::Acceptor* acceptor, int fd, const char* ip, int port)
{

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