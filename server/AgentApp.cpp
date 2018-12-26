#include "AgentApp.h"

AgentApp::AgentApp(std::string file):file_(file) {
	clientAcceptor_ = new Network::Acceptor(poller_);
}

AgentApp::~AgentApp() {

}

int AgentApp::Init()
{
	LOG_ERROR(fmt::format("AgentApp start:{}", file_));
	ServerApp::Init();

	Network::Acceptor::OnConnection callback = std::bind(&AgentApp::OnClientAccept, this, std::placeholders::_1, std::placeholders::_2);
	clientAcceptor_->SetCallback(callback);

	return 0;
}