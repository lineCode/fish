#ifndef LUAFISH_H
#define LUAFISH_H

extern "C"
{
#include "lua.h"
#include "lualib.h"
};
class ServerApp;

class LuaFish
{
public:
	enum SocketEvent
	{
		Data = 1,
		Accept,
		Connect,
		Close
	};
public:
	LuaFish(void);
	~LuaFish(void);

	lua_State* LuaState();

	void Require(const char* module,int (*func)(lua_State*));

	int Init(ServerApp* app);

	int DoFile(const char* file);

	int DispatchClient(int source,int method,const char* data,int size);

	int DispatchServerEvent(int source,bool start = true);

	int DispatchServer(int source,const char* data,int size);

	int DispatchSocketEvent(int source,SocketEvent ev,int reserve = 0);

	int DispatchSocket(int source,const char* data,int size);

	int DispatchMongo(int source,int session,const char* data,int size);

	int DispatchTimer(int session);

	int GenSession();

	void CallBack(int index);

	void LuaPath(const char* path);

	static int Register(lua_State* L);
	static int _Traceback(lua_State* L);
	static int _Send(lua_State* L);
	static int _SendClient(lua_State* L);
	static int _Log(lua_State* L);
	static int _Now(lua_State* L);
	static int _Timestamp(lua_State* L);
	static int _TimestampToSecond(lua_State* L);
	static int _GenSession(lua_State* L);
	static int _CallBack(lua_State* L);
	static int _ConfigInit(lua_State* L);
protected:
	lua_State*	_L;
	int			_callback;
	int			_sessionCounter;
};

#endif