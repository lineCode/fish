#include "ClientChannel.h"
#include "ClientManager.h"
#include "logger/Logger.h"
#include "util/Util.h"
#include "AgentApp.h"

#define HEADER_SIZE 2
#define MAX_MESSAGE_SIZE 1024 * 16

ClientChannel::ClientChannel(Network::EventPoller* poller, int fd, int vid) : Super(poller, fd) {
	vid_ = vid;
	freq_ = 0;
	need_ = 0;
	seed_ = 0;
	lastMsgTime_ = 0;

	using namespace std::placeholders;
	timer_ = Timer::AssignTimer();
	timer_->SetCallback(std::bind(&ClientChannel::OnUpdate, this, _1, _2));
	timer_->Start(poller, 1, 1);

	OOLUA::Script& script = APP->Lua()->GetScript();
	if ( !script.call("OnClientEnter", vid_) ) {
		LOG_ERROR(fmt::format("OnClientEnter error:{}", OOLUA::get_last_error(script)));
	}
}


ClientChannel::~ClientChannel() {
	timer_->Cancel();
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
				LOG_ERROR(fmt::format("client:{} receive message:{} too much", vid_, need_));
				OnClientError(true);
				return;
			}
		} else {
			if (reader_->total_ < (int)need_) {
				return;
			}

			uint8_t* data = CLIENT_MGR->AllocBuffer(need_);
			reader_->ReadData((char*)data, need_);

			if (Util::MessageDecrypt(&seed_, data, need_) < 0) {
				LOG_ERROR(fmt::format("client:{} message decrypt error", vid_));
				CLIENT_MGR->FreeBuffer(data);
				OnClientError(true);
				return;
			}

			uint16_t msgId = data[2] | data[3] << 8;

			OOLUA::Script& script = APP->Lua()->GetScript();
			if ( !script.call("OnClientData", vid_, msgId, &data[4], need_ - 4) ) {
				LOG_ERROR(fmt::format("OnClientData error:{}", OOLUA::get_last_error(script)));
			}
			
			CLIENT_MGR->FreeBuffer(data);

			need_ = 0;
			lastMsgTime_ = APP->Now();
			freq_++;
		}
	}
}

void ClientChannel::HandleClose() {
	CLIENT_MGR->DeleteClient(vid_);
	CLIENT_MGR->MarkClientDead(this);
}

void ClientChannel::HandleError() {
	OnClientError(false);
}

void ClientChannel::OnUpdate(Timer* timer, void* userdata) {
	if (reader_->total_ > (int)CLIENT_MGR->GetWarnFlow()) {
		LOG_ERROR(fmt::format("client:{} more than {}kb need to send out", vid_, reader_->total_ / 1024));
	}

	bool error = false;
	if (freq_ >= CLIENT_MGR->GetMaxFreq()) {
		LOG_ERROR(fmt::format("client:{} receive message too much:{} in last 1s", vid_, freq_));
		error = true;
	} else {
		freq_ = 0;
		if (lastMsgTime_ != 0 && APP->Now() - lastMsgTime_ > CLIENT_MGR->GetMaxAlive()) {
			LOG_ERROR(fmt::format("client:{} time out", vid_));
			error = true;
		}
	}

	if (error) {
		OnClientError(true);
	}
}

void ClientChannel::OnClientError(bool close) {
	if (close) {
		Close(true);
	} else {
		CLIENT_MGR->DeleteClient(vid_);
		CLIENT_MGR->MarkClientDead(this);

	}
	
	OOLUA::Script& script = APP->Lua()->GetScript();
	if ( !script.call("OnClientError", vid_) ) {
		LOG_ERROR(fmt::format("OnClientError error:{}", OOLUA::get_last_error(script)));
	}
}

int ClientChannel::GetVid() {
	return vid_;
}
