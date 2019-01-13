#include "LoggerClient.h"
#include "network/Connector.h"
#include "util/format.h"
#include "util/Util.h"
#include <iostream>
#include <assert.h>

LoggerClient::LoggerClient(Network::Addr& addr, Network::EventPoller* poller):addr_(addr) {
	poller_ = poller;
	
	Network::Connector connector(poller_);
	int fd = connector.Connect(addr_, false);
	assert(fd > 0);

	channel_ = new LoggerChannel(poller_, fd);
	channel_->SetCloseCallback(std::bind(&LoggerClient::OnChannelClose, this, std::placeholders::_1));

	timer_ = new Timer();
	timer_->SetCallback(std::bind(&LoggerClient::OnUpdate, this, std::placeholders::_1, std::placeholders::_2));
	timer_->Start(poller_, 1, 1);
}

LoggerClient::~LoggerClient(void) {
	if (channel_) {
		delete channel_;
	}
	if (timer_) {
		delete timer_;
	}
}

void LoggerClient::RuntimeLog(std::string& log) {
	WriteLog("runtime", log);
}

void LoggerClient::WriteLog(const char* file, std::string& log) {
	MemoryStream ms;
	ms << (int32_t)0 << file << log;
	if (channel_) {
		size_t size;
		char* message = Util::MakeMessage(ms, &size);
		channel_->Write(message, size);
	} else {
		cached_.push_back(ms);
	}
}

void LoggerClient::WriteLog(const char* file, void* data, size_t size) {
	MemoryStream ms;
	ms << (int32_t)1 << file << size;
	ms.Append((const char*)data, size);

	free(data);

	if (channel_) {
		size_t size;
		char* message = Util::MakeMessage(ms, &size);
		channel_->Write(message, size);
	} else {
		cached_.push_back(ms);
	}
}

void LoggerClient::OnChannelClose(void* userdata) {
	delete channel_;
	channel_ = NULL;
}

void LoggerClient::OnUpdate(Timer* timer, void* userdata) {
	if (channel_) {
		return;
	}

	Network::Connector connector(poller_);
	int fd = connector.Connect((const Network::Addr&)addr_, false);
	if (fd < 0) {
		return;
	}

	channel_ = new LoggerChannel(poller_, fd);
	channel_->SetCloseCallback(std::bind(&LoggerClient::OnChannelClose, this, std::placeholders::_1));

	std::vector<MemoryStream>::iterator iter = cached_.begin();
	for(;iter != cached_.end();iter++) {
		MemoryStream& ms = *iter;
		size_t size;
		char* message = Util::MakeMessage(ms, &size);
		channel_->Write(message, size);
	}
	cached_.clear();
}

