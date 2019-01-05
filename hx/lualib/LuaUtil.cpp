#include "LuaUtil.h"
#include "ServerApp.h"
#include "util/MemoryStream.h"

int LuaUtil::Register(lua_State* L) {
	luaL_checkversion(L);

	luaL_Reg methods[] = {
		{ "ParseLoggerMessage", LuaUtil::ParseLoggerMessage },
		{ NULL, NULL },
	};

	luaL_newlibtable(L, methods);

	lua_getfield(L, LUA_REGISTRYINDEX, "app");
	ServerApp *app = (ServerApp*)lua_touserdata(L,-1);
	if (app == NULL) {
		return luaL_error(L, "Init ServerApp context first");
	}

	luaL_setfuncs(L, methods, 1);

	return 1;
}

int LuaUtil::ParseLoggerMessage(lua_State* L) {
	char* data = (char*)lua_touserdata(L, 1);
	int size = luaL_checkinteger(L, 2);

	MemoryStream ms(data, size);

	std::string file;
	std::string log;

	ms >> file >> log;

	lua_pushlstring(L, file.c_str(), file.length());
	lua_pushlstring(L, log.c_str(), log.length());

	return 2;
}
