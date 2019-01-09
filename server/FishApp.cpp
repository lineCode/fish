#include "FishApp.h"
#include "logger/Logger.h"
#include "util/format.h"
#include "network/HttpChannel.h"

template <>
FishApp * Singleton<FishApp>::singleton_ = 0;

FishApp::FishApp(Network::EventPoller* poller) : ServerApp(poller) {
}

FishApp::~FishApp(void) {
}

int FishApp::Init(std::string& boot) {
	LOG_ERROR(std::string("FishApp start"));
	ServerApp::Init(boot);
	return 0;
}

int FishApp::Fina() {
	return ServerApp::Fina();
}

int FishApp::Run() {
	return ServerApp::Run();
}

void FishApp::OnUpate(Timer* timer, void* userdata) {
	ServerApp::OnUpate(timer, userdata);
}
