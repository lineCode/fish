#ifndef LUACONNECTOR_H
#define LUACONNECTOR_H

#include "LuaSocket.h"

#include "../network/EventPoller.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
};

class LuaConnector : public Network::OutputHandler,public Network::ErrorHandler
{
public:
	LuaConnector(ServerApp* app,Network::EventPoller* poller);
	~LuaConnector(void);

	virtual int		Connect(const char * host,int port,bool& done);

	virtual int		HandleOutput();

	virtual int 	HandleError();

	static int		Register(lua_State* L);
	static int		_New(lua_State* L);
	static int		_Delete(lua_State* L);
	static int		_Connect(lua_State* L);

protected:
	ServerApp*				_app;
	Network::EventPoller*	_poller;
	std::string				_host;
	int						_port;
	int						_fd;
	int						_id;
};

#endif