#ifndef LUAACCEPTOR_H
#define LUAACCEPTOR_H

#include "LuaSocket.h"
#include "../network/Interface.h"
#include "../network/EventPoller.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
};

class LuaAcceptor : public Network::InputHandler 
{
public:
	LuaAcceptor(ServerApp* app,Network::EventPoller* poller);
	~LuaAcceptor(void);

	virtual int		Listen(const char * host,int port);
	virtual int		HandleInput();

	static int		Register(lua_State* L);
	static int		_New(lua_State* L);
	static int		_Delete(lua_State* L);
	static int		_Listen(lua_State* L);
	static int		_Close(lua_State* L);

protected:
	ServerApp*				_app;
	Network::EventPoller*	_poller;
	std::string				_host;
	int						_port;
	int						_fd;
	int						_id;
	bool					_listening;
};

#endif