#include "LuaTimer.h"


LuaTimer::LuaTimer(ServerApp* app)
{
	_app = app;
	_session = 0;
}


LuaTimer::~LuaTimer(void)
{
}

int LuaTimer::HandleTimeout()
{
	this->_app->LuaManager()->DispatchTimer(_session);
	return -1;
}

void LuaTimer::SetSession(int session)
{
	_session = session;
}

int LuaTimer::Register(lua_State* L)
{
	luaL_checkversion(L);

	luaL_Reg l[] =
	{
		{ "New", LuaTimer::_New },
		{ "Delete", LuaTimer::_Delete },
		{ "Start", LuaTimer::_Start },
		{ NULL, NULL },
	};

	luaL_newlibtable(L, l);

	lua_getfield(L, LUA_REGISTRYINDEX, "CoreCtx");
	ServerApp *app = (ServerApp*)lua_touserdata(L,-1);
	if (app == NULL)
		return luaL_error(L, "Init ServerApp context first");

	luaL_setfuncs(L,l,1);

	return 1;
}

int LuaTimer::_New(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	LuaTimer* timer = new LuaTimer(app);
	lua_pushlightuserdata(L,timer);
	return 1;
}

int LuaTimer::_Delete(lua_State* L)
{
	LuaTimer* timer = (LuaTimer*)lua_touserdata(L,1);
	delete timer;
	return 0;
}

int LuaTimer::_Start(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

	LuaTimer* timer = (LuaTimer*)lua_touserdata(L,1);
	int ti = lua_tointeger(L,2);
	int session = lua_tointeger(L,3);

	timer->_session = session;

	app->Poller()->Timer().Register(timer,ti);

	return 0;
}
