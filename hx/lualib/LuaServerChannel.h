#ifndef LUA_SERVERCHANNEL_H
#define LUA_SERVERCHANNEL_H

#include "network/Channel.h"

class LuaServerChannel : public Channel {
public:
	typedef std::function<void(int, int, char*, size_t)> OnData;
	typedef std::function<void(int, int, const char*)> OnError;

public:
	LuaServerChannel(Network::EventPoller* poller, int fd, int reference);

	virtual ~LuaServerChannel();

	virtual void HandleRead();

	virtual void HandleClose();

	virtual void HandleError();

	void SetDataCallback(OnData callback);

	void SetErrorCallback(OnError callback);

	void SetDataReference(int reference);
	
	int GetDataReference();
	
	int GetErrorReference();

	void SetErrorReference(int reference);

	int GetReference();

private:
	OnData dataCallback_;
	int dataRefrence_;
	
	OnError errorCallback_;
	int errorRefrence_;
	
	int reference_;
};

#endif