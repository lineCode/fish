#include "ClientChannel.h"
#include "ClientManager.h"

#define HEADER_SIZE 2
#define MAX_MESSAGE_SIZE 16 * 1024

ClientChannel::ClientChannel(Network::EventPoller* poller, int fd) :Super(poller, fd) {
	id_ = 0;
	freq_ = 0;
	need_ = 0;
	seed_ = 0;
	lastMsgTime_ = 0;
}


ClientChannel::~ClientChannel() {
}

void ClientChannel::HandleRead() {
	while(IsAlive()) {
		if (need_ == 0) {
			uint8_t header[HEADER_SIZE];
			ReadData(header, HEADER_SIZE);

			need_ = header[0] | header[1] << 8;
			need_ -= HEADER_SIZE;

			if (need_ > MAX_MESSAGE_SIZE) {
				LOG_ERROR(std::string("client:{} receive message:{} too much", id_, need_));
				Close(true);
				OnClientError();
				return;
			}
		} else {

		}
	}
}

void ClientChannel::HandleClose() {
	ClientManager::GetSingleton()->DeleteClient(id_);
}

void ClientChannel::HandleError() {
	OnClientError();
}

void ClientChannel::OnClientError() {
	ClientManager::GetSingleton()->DeleteClient(id_);
}

void ClientChannel::SetId(int id) {
	id_ = id;
}

int ClientChannel::GetId() {
	return id_;
}