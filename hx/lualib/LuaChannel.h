#ifndef LUA_CHANNEL_H
#define LUA_CHANNEL_H

#include "network/Channel.h"

class LuaChannel : public Network::Channel {
public:
	LuaChannel(Network::EventPoller* poller,int fd, LuaFish* lua, uint32_t header);

	virtual ~LuaChannel();

	void UnRefAll();

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
	
	static int LuaRead(lua_State* L);
	static int LuaWrite(lua_State* L);
	static int LuaClose(lua_State* L);
	static int LuaRelease(lua_State* L);
private:
	LuaFish* lua_;
	uint32_t header_;
	uint32_t need_;
	int reference_;
	int dataReference_;
	int closeReference_;
	int errorReference_;
};

#endif
