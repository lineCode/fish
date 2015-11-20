#ifndef LUAHTTPPARSER_H
#define LUAHTTPPARSER_H

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

namespace LuaHttpParser
{
	int Register(lua_State* L);
}

#endif
