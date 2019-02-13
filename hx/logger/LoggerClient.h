#ifndef LOGGER_CLIENT_H
#define LOGGER_CLIENT_H

#include "LoggerInterface.h"
#include "network/Address.h"
#include "network/EventPoller.h"
#include "LoggerChannel.h"
#include "time/Timer.h"
#include "util/StreamWriter.h"
#include <vector>

class LoggerClient : public LoggerInterface {
public:
	LoggerClient(Network::Addr& addr, Network::EventPoller* poller);

	virtual ~LoggerClient();

	virtual void WriteLog(const char* file, const char* source, int line, int level, uint64_t time, const char* content);

	virtual void WriteLog(const char* file, void* data, size_t size);

	void OnChannelClose(void* userdata);

	void OnUpdate(Timer* timer, void* userdata);

private:
	bool Connect();

private:
	Network::Addr addr_;
	LoggerChannel* channel_;
	Timer* timer_;
	Network::EventPoller* poller_;
	std::vector<StreamWriter> cached_;
};
#endif
