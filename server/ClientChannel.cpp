#include "ClientChannel.h"
#include "ClientManager.h"
#include "logger/Logger.h"
#include "util/Util.h"
#include "FishApp.h"

#define HEADER_SIZE 2
#define MAX_MESSAGE_SIZE 1024 * 16
#define WARN_WRITER_TOTAL 1024 * 10
#define MAX_FREQ 200
#define MAX_ALIVE_TIME 60 * 3

ClientChannel::ClientChannel(Network::EventPoller* poller, int fd, int id) :Super(poller, fd) {
	id_ = id;
	freq_ = 0;
	need_ = 0;
	seed_ = 0;
	lastMsgTime_ = 0;

	using namespace std::placeholders;
	timer_ = Timer::AssignTimer();
	timer_->SetCallback(std::bind(&ClientChannel::OnUpdate, this, _1, _2));
	timer_->Start(poller, 1, 1);
}


ClientChannel::~ClientChannel() {
	Timer::ReclaimTimer(timer_);
}

void ClientChannel::HandleRead() {
	while(IsAlive()) {
		if (need_ == 0) {
			if (reader_->total_ < HEADER_SIZE) {
				return;
			}

			uint8_t header[HEADER_SIZE];
			reader_->ReadData((char*)header, HEADER_SIZE);

			need_ = header[0] | header[1] << 8;
			need_ -= HEADER_SIZE;

			if (need_ > MAX_MESSAGE_SIZE) {
				LOG_ERROR(fmt::format("client:{} receive message:{} too much", id_, need_));
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

			if (Util::MessageDecrypt(&seed_, data, need_) < 0) {
				LOG_ERROR(fmt::format("client:{} message decrypt error", id_));
				free(data);
				Close(true);
				OnClientError();
				return;
			}
			
			uint16_t id = data[2] | data[3] << 8;
			
			free(data);

			need_ = 0;
			lastMsgTime_ = APP->Now();
			freq_++;
		}
	}
}

void ClientChannel::HandleClose() {
	CLIENT_MGR->DeleteClient(id_);
	CLIENT_MGR->MarkClientDead(this);
}

void ClientChannel::HandleError() {
	OnClientError();
}

void ClientChannel::OnUpdate(Timer* timer, void* userdata) {
	if (reader_->total_ > WARN_WRITER_TOTAL) {
		LOG_ERROR(fmt::format("client:{} more than {}kb need to send out", id_, reader_->total_));
	}

	bool error = false;
	if (freq_ >= MAX_FREQ) {
		LOG_ERROR(fmt::format("client:{} receive message too much:{} in last 1s", id_, freq_));
		error = true;
	} else {
		freq_ = 0;
		if (lastMsgTime_ != 0 && APP->Now() - lastMsgTime_ > MAX_ALIVE_TIME) {
			LOG_ERROR(fmt::format("client:{} time out", id_));
			error = true;
		}
	}

	if (error) {
		Close(true);
		OnClientError();
	}
}

void ClientChannel::OnClientError() {
	CLIENT_MGR->DeleteClient(id_);
	CLIENT_MGR->MarkClientDead(this);
}

int ClientChannel::GetId() {
	return id_;
}