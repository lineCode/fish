
#ifndef LUAMONGO_H
#define LUAMONGO_H
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

namespace LuaMongo
{
	int Register(lua_State* L);
}
#endif