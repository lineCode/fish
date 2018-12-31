#ifndef LUA_CHANNEL_H
#define LUA_CHANNEL_H

#include "network/Channel.h"

class LuaChannel : public Network::Channel {
public:
	LuaChannel(Network::EventPoller* poller,int fd, LuaFish* lua, uint32_t header);

	virtual ~LuaChannel();

	virtual void HandleRead();
	virtual void HandleClose();
	virtual void HandleError();

	void SetReference(int reference);
	int GetReference();

	void SetDataReference(int reference);
	int GetDataReference();

	void SetCloseReference(int reference);
	int GetCloseReference();

	void SetErrorReference(int reference);
	int GetErrorReference();
	
	static int Read(lua_State* L);
	static int Write(lua_State* L);
	static int Close(lua_State* L);
	static int Release(lua_State* L);
private:
	LuaFish* lua_;
	uint32_t header_;
	uint32_t need_;
	int reference_;
	int dataRefernce_;
	int closeRefernce_;
	int errorRefernce_;
};

#endif
