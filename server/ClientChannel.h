#ifndef CLIENT_CHANNEL_H
#define CLIENT_CHANNEL_H

#include "time/Timer.h"
#include "network/Channel.h"

class ClientChannel : public Network::Channel {
public:
	typedef Network::Channel Super;

public:
	ClientChannel(Network::EventPoller* poller, int fd, int id);

	~ClientChannel();

	virtual void HandleRead();
	virtual void HandleClose();
	virtual void HandleError();

	void OnUpdate(Timer* timer, void* userdata);

	void OnClientError(bool close = false);

	int GetId();

private:
	int id_;
	uint32_t freq_;
	uint32_t need_;
	uint16_t seed_;
	uint64_t lastMsgTime_;
	Timer* timer_;
};

#endif