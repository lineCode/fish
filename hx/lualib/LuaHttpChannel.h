#ifndef LUA_HTTP_CHANNEL_H
#define LUA_HTTP_CHANNEL_H
#include "network/HttpChannel.h"

class LuaHttpChannel : public Network::HttpChannel {
public:
	LuaHttpChannel(Network::EventPoller* poller,int fd, LuaFish* lua);

	virtual ~LuaHttpChannel();

	virtual void HandleRead();
	virtual void HandleClose();
	virtual void HandleError();

	void SetReference(int reference);
	int GetReference();

	void SetCallback(int callback);
	
	static int LuaGetUrl(lua_State* L);
	static int LuaGetHeader(lua_State* L);
	static int LuaGetContent(lua_State* L);
	static int LuaSetReplyHeader(lua_State* L);
	static int LuaReply(lua_State* L);
	static int LuaClose(lua_State* L);
	static int LuaRelease(lua_State* L);
private:
	LuaFish* lua_;
	int reference_;
	int callback_;
};

#endif