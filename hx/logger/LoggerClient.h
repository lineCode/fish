#ifndef LOGGER_CLIENT_H
#define LOGGER_CLIENT_H

#include "LoggerInterface.h"
#include "network/Address.h"
#include "network/EventPoller.h"
#include "LoggerChannel.h"
#include "time/Timer.h"
#include "util/MemoryStream.h"
#include <vector>

class LoggerClient : public LoggerInterface {
public:
	LoggerClient(Network::Addr& addr, Network::EventPoller* poller);

	virtual ~LoggerClient();

	virtual void RuntimeLog(std::string& log);

	virtual void LuaLog(const char* file, std::string& log);

	void OnChannelClose(void* userdata);

	void OnUpdate(Timer* timer, void* userdata);

private:
	Network::Addr addr_;
	LoggerChannel* channel_;
	Timer* timer_;
	Network::EventPoller* poller_;
	std::vector<MemoryStream> cached_;
};
#endif
