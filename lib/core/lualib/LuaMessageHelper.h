#ifndef LUAMESSAGEHELPER_H
#define LUAMESSAGEHELPER_H

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};


namespace LuaMessageHelper
{
	int Register(lua_State* L);
}
#endif