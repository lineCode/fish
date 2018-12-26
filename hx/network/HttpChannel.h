#include "Channel.h"

namespace Network {

class HttpChannel : public Channel {
public:
	HttpChannel(Network::EventPoller* poller,int fd);

	virtual ~HttpChannel();

	virtual void HandleRead();

	virtual void HandleWrite();

	virtual void HandleError();
}
};