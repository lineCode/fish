#ifndef LOGGER_CHANNEL_H
#define LOGGER_CHANNEL_H
#include "network/Channel.h"

class LoggerChannel : public Network::Channel {
public:
	typedef std::function<void(void*)> Callback;

public:
	LoggerChannel(Network::EventPoller* poller, int fd);
	virtual ~LoggerChannel();

	virtual void HandleClose();
	virtual void HandleError();

	void SetCloseCallback(Callback callback);
	void SetUserdata(void* userdata);
private:
	Callback callback_;
	void* userdata_;
};

#endif
