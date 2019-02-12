#include "LoggerClient.h"
#include "network/Connector.h"
#include "network/TcpReader.h"
#include "network/TcpWriter.h"
#include "util/format.h"
#include "util/Util.h"
#include "logger/Logger.h"
#include <iostream>
#include <assert.h>

LoggerClient::LoggerClient(Network::Addr& addr, Network::EventPoller* poller):addr_(addr) {
	poller_ = poller;
	
	Network::Connector connector(poller_);
	int fd = connector.Connect(addr_, false);
	assert(fd > 0);

	channel_ = new LoggerChannel(poller_, fd);
	channel->SetReader(new Network::TcpReader(64));
	channel->SetWriter(new Network::TcpWriter());
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

void LoggerClient::WriteLog(const char* file, const char* source, int line, int level, uint64_t time, const char* content) {
	StreamWriter writer;
	writer << (int32_t)0 << file << source << line << level << time << content;
	if (channel_) {
		size_t size;
		char* message = Util::MakeMessage(writer, &size);
		channel_->Write(message, size, NULL);
	} else {
		cached_.push_back(writer);
	}
}

void LoggerClient::WriteLog(const char* file, void* data, size_t size) {
	StreamWriter writer;
	writer << (int32_t)1 << file << size;
	writer.Append((const char*)data, size);

	free(data);

	if (channel_) {
		size_t size;
		char* message = Util::MakeMessage(writer, &size);
		channel_->Write(message, size, NULL);
	} else {
		cached_.push_back(writer);
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

	std::vector<StreamWriter>::iterator iter = cached_.begin();
	for(;iter != cached_.end();iter++) {
		StreamWriter& writer = *iter;
		size_t size;
		char* message = Util::MakeMessage(writer, &size);
		channel_->Write(message, size, NULL);
	}
	cached_.clear();
}

