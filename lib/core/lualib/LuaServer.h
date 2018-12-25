#ifndef LUASERVER_H
#define LUASERVER_H

#include "../network/Channel.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
};


class LuaServer : public Network::Channel
{
public:
	LuaServer(ServerApp* app,Network::EventPoller * poller,int fd);
	~LuaServer(void);

	virtual int Init();

	virtual int Fina();

	virtual int Forward(const char * ptr,int size);

	static int		Register(lua_State* L);
	static int		_New(lua_State* L);
	static int		_Delete(lua_State* L);
	static int		_Close(lua_State* L);

protected:
	ServerApp*	_app;
};

#endif

