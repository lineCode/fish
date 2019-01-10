﻿#include "AgentApp.h"
#include "logger/Logger.h"
#include "util/format.h"
#include "util/Util.h"
#include "network/HttpChannel.h"



template <>
AgentApp * Singleton<AgentApp>::singleton_ = 0;

AgentApp::AgentApp(Network::EventPoller* poller) : ServerApp(poller) {
}

AgentApp::~AgentApp(void) {
	delete clientMgr_;
}

int AgentApp::Init(rapidjson::Document& config) {
	LOG_ERROR(fmt::format("AgentApp start"));

	std::string boot("agent");
	ServerApp::Init(boot);

	lua_->DoFile("client");
	
	if (!config.HasMember("clientAddr")) {
		Util::Exit("no client addr");
	}

	int maxClient = 1000;
	if (config.HasMember("maxClient")) {
		maxClient = config["maxClient"].GetInt();
	}

	clientMgr_ = new ClientManager(maxClient, 1);

	if (config.HasMember("maxFreq")) {
		clientMgr_->SetMaxFreq(config["maxFreq"].GetInt());
	}

	if (config.HasMember("maxAlive")) {
		clientMgr_->SetMaxAlive(config["maxAlive"].GetInt());
	}

	if (config.HasMember("warnFlow")) {
		clientMgr_->SetWarnFlow(config["warnFlow"].GetInt());
	}

	const char* ip = config["clientAddr"]["ip"].GetString();
	int port = config["clientAddr"]["port"].GetInt();

	Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
	if (clientMgr_->Start(addr) < 0) {
		Util::Exit(fmt::format("client manager start:{} error", addr.ToStr()));
	}

	lua_->Require("clientMgr", ClientManager::Register);

	return 0;
}

int AgentApp::Fina() {
	return ServerApp::Fina();
}

int AgentApp::Run() {
	return ServerApp::Run();
}

void AgentApp::OnUpate(Timer* timer, void* userdata) {
	ServerApp::OnUpate(timer, userdata);
}
