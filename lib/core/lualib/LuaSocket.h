#ifndef LUASOCKET_H
#define LUASOCKET_H

#include "../network/Channel.h"
#include "../util/List.h"
extern "C"
{
#include <lua.h>
#include <lauxlib.h>
};


class LuaSocket : public Network::Channel
{
public:
	typedef List<MemoryStream> StreamList;
public:
	LuaSocket(ServerApp* app,Network::EventPoller * poller,int fd);
	~LuaSocket(void);

	virtual int Init();
	
	virtual int Fina();
	
	virtual int Close();
	
	virtual int Forward(const char * ptr,int size);

	void PopStream(MemoryStream*& ms);
	void PushStream(MemoryStream* ms);

	static int Register(lua_State* L);
	static int _New(lua_State* L);
	static int _Delete(lua_State* L);
	static int _SaveBuffer(lua_State* L);
	static int _ReadBuffer(lua_State* L);

	static int _Send(lua_State* L);
	static int _Close(lua_State* L);

protected:
	ServerApp*	_app;
	int			_total;
	StreamList	_waitlist;
	StreamList	_freelist;
};

#endif

