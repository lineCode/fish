#include "ClientChannel.h"
#include "ClientManager.h"

ClientChannel::ClientChannel(Network::EventPoller* poller, int fd) :Super(poller, fd) {
	id_ = 0;
}


ClientChannel::~ClientChannel() {
}

void ClientChannel::HandleRead() {

}

void ClientChannel::HandleClose() {
	ClientManager::GetSingleton()->DeleteClient(id_);
}

void ClientChannel::HandleError() {
	ClientManager::GetSingleton()->DeleteClient(id_);
}

void ClientChannel::SetId(int id) {
	id_ = id;
}

int ClientChannel::GetId() {
	return id_;
}