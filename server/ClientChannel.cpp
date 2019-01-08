#include "ClientChannel.h"
#include "ClientManager.h"
#include "logger/Logger.h"
#include "FishApp.h"

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
			reader_->ReadData((char*)header, HEADER_SIZE);

			need_ = header[0] | header[1] << 8;
			need_ -= HEADER_SIZE;

			if (need_ > MAX_MESSAGE_SIZE) {
				LOG_ERROR(std::string("client:{} receive message:{} too much", id_, need_));
				Close(true);
				OnClientError();
				return;
			}
		} else {
			if (reader_->total_ < need_) {
				return;
			}

			uint8_t* data = (uint8_t*)malloc(need_);
			reader_->ReadData((char*)data, need_);
			free(data);

			need_ = 0;
			lastMsgTime_ = FishApp::GetSingleton()->Now();
			freq_++;
		}
	}
}

void ClientChannel::HandleClose() {
	ClientManager::GetSingleton()->DeleteClient(id_);
}

void ClientChannel::HandleError() {
	OnClientError();
}

void ClientChannel::OnUpdate() {

}

void ClientChannel::OnClientError() {
	ClientManager::GetSingleton()->DeleteClient(id_);
	ClientManager::GetSingleton()->MarkClientDead(this);
}

void ClientChannel::SetId(int id) {
	id_ = id;
}

int ClientChannel::GetId() {
	return id_;
}