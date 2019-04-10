#include "LoginApp.h"
#include "logger/Logger.h"
#include "util/format.h"
#include "util/Util.h"
#include "network/HttpChannel.h"



template <>
LoginApp * Singleton<LoginApp>::singleton_ = 0;

LoginApp::LoginApp(Network::EventPoller* poller) : ServerApp("login", poller) {
}

LoginApp::~LoginApp(void) {
	delete clientMgr_;
}

int LoginApp::Init(const rapidjson::Value& config) {
	LOG_ERROR(fmt::format("LoginApp start"));

	ServerApp::Init("login");

	int maxClient = 1000;
	if (config.HasMember("maxClient")) {
		maxClient = config["maxClient"].GetInt();
	}

	clientMgr_ = new Network::ClientManager(this, maxClient, 1);

	if (config.HasMember("maxFreq")) {
		clientMgr_->SetMaxFreq(config["maxFreq"].GetInt());
	}

	if (config.HasMember("maxAlive")) {
		clientMgr_->SetMaxAlive(config["maxAlive"].GetInt());
	}

	if (config.HasMember("warnFlow")) {
		clientMgr_->SetWarnFlow(config["warnFlow"].GetInt());
	}

	const char* ip = config["ip"].GetString();
	int port = config["port"].GetInt();

	Network::Addr addr = Network::Addr::MakeIP4Addr(ip, port);
	if (clientMgr_->Start(addr) < 0) {
		Util::Exit(fmt::format("client manager start:{} error", addr.ToStr()));
	}

	return 0;
}


