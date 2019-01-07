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
	
	static int LGetUrl(lua_State* L);
	static int LGetHeader(lua_State* L);
	static int LGetContent(lua_State* L);
	static int LSetReplyHeader(lua_State* L);
	static int LReply(lua_State* L);
	static int LClose(lua_State* L);
	static int LRelease(lua_State* L);
private:
	LuaFish* lua_;
	int reference_;
	int callback_;
};

#endif