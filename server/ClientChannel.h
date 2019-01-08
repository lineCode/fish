#ifndef CLIENT_CHANNEL_H
#define CLIENT_CHANNEL_H

#include "network/Channel.h"

class ClientChannel : public Network::Channel {
public:
	typedef Network::Channel Super;

public:
	ClientChannel(Network::EventPoller* poller, int fd);
	~ClientChannel();

	virtual void HandleRead();
	virtual void HandleClose();
	virtual void HandleError();

	void SetId(int id);
	int GetId();

private:
	int id_;
};

#endif