#ifndef LUA_UTIL_H
#define LUA_UTIL_H

#include "lua.hpp"



class LuaUtil {
public:
	static int Register(lua_State* L);
	static int ParseLoggerMessage(lua_State* L);
	static int Readline(lua_State* L);
};

#endif
