#ifndef LUAPROF_H
#define LUAPROF_H
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

namespace LuaProf
{
	int luaopen_prof(lua_State* L);
}
#endif