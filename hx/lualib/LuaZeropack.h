#ifndef LUAZEROPACK_H
#define LUAZEROPACK_H

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};


namespace LuaZeropack
{
	int Register(lua_State* L);
}

#endif