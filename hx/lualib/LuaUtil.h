#ifndef LUA_UTIL_H
#define LUA_UTIL_H

#include "lua.hpp"



class LuaUtil {
	static int Register(lua_State* L);
	static int ParseLoggerMessage(lua_State* L);
};

#endif
