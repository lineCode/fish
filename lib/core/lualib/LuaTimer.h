#ifndef LUATIMER_H
#define LUATIMER_H

#include "../ServerApp.h"
#include "../time/TimerHandler.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
};

class LuaTimer : public TimeoutHandler
{
public:
	LuaTimer(ServerApp* app);
	~LuaTimer(void);

	int HandleTimeout();

	void SetSession(int session);

	static int Register(lua_State* L);
	static int _New(lua_State* L);
	static int _Delete(lua_State* L);
	static int _Start(lua_State* L);

protected:
	int _session;
	ServerApp* _app;
};


#endif
