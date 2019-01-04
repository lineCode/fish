#include "Logger.h"
#include "network/Connector.h"
#include "util/format.h"
#include <iostream>
#include <assert.h>

template <> 
Logger * Singleton<Logger>::singleton_ = 0;

Logger::Loglevel Logger::level_ = Trace;

//as logger client
Logger::Logger(Network::Addr& addr, Network::EventPoller* poller):addr_(addr) {
	poller_ = poller;
	
	Network::Connector connector(poller_);
	int fd = connector.Connect(addr_, false);
	assert(fd > 0);

	channel_ = new LoggerChannel(poller_, fd);
	channel_->SetCloseCallback(std::bind(&Logger::OnChannelClose, this, std::placeholders::_1));

	timer_ = new Timer();
	timer_->SetCallback(std::bind(&Logger::OnUpdate, this, std::placeholders::_1, std::placeholders::_2));
	timer_->Start(app->Poller(), 1, 1);

	FILE_ = NULL;
}

//as logger server
Logger::Logger(const char* file) {
	channel_ = NULL;
	timer_ = NULL;
	poller_ = NULL;

	if (file != NULL) {
		FILE_ = fopen(file,"w");
		assert(FILE_ != NULL);
	} else {
		FILE_ = stderr;
	}
}

Logger::~Logger(void) {
	if (channel_) {
		delete channel_;
	}
	if (timer_) {
		delete timer_;
	}
	if (FILE_) {
		fflush(FILE_);
		fclose(FILE_);
	}
}

void Logger::Log(const char* file,int line,Loglevel level,const char* content) {
	if (level < Logger::level_) {
		return;
	}
	std::lock_guard<std::mutex> guard(mutex_);
	std::string log = fmt::format("@{}:{}: {}\n", file, line, content);

	if (FILE_) {
		fwrite(log.c_str(), log.length() , 1, FILE_);
	} else {
		if (channel_) {
			channel_->Write(log);
		} else {
			cached_.push_back(log);
		}
	}
}

void Logger::LuaLog(const char* content) {
	std::lock_guard<std::mutex> guard(mutex_);
	std::string log = fmt::format("@lua: {}\n",content);
	if (FILE_) {
		fwrite(log.c_str(), log.length() , 1, FILE_);
	} else {
		if (channel_) {
			channel_->Write(log);
		} else {
			cached_.push_back(log);
		}
	}
}

void Logger::OnChannelClose(void* userdata) {
	delete channel_;
	channel_ = NULL;
}

void Logger::OnUpdate(Timer* timer, void* userdata) {
	if (!channel_) {
		return;
	}

	Network::Connector connector(poller_);
	int fd = connector.Connect((const Network::Addr&)addr_, false);
	if (fd < 0) {
		return;
	}

	channel_ = new LoggerChannel(poller_, fd);
	channel_->SetCloseCallback(std::bind(&Logger::OnChannelClose, this, std::placeholders::_1));

	std::vector<std::string>::iterator iter = cached_.begin();
	for(;iter != cached_.end();iter++) {
		std::string log = *iter;
		channel_->Write(log);
	}
	cached_.clear();
}

void Logger::SetLogLevel(Loglevel level) {
	Logger::level_ = level;
}

Logger::Loglevel Logger::LogLevel() {
	return Logger::level_;
}
