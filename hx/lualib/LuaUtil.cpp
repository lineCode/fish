#include "LuaUtil.h"
#include "util/MemoryStream.h"

int ParseLoggerMessage(lua_State* L) {
	char* data = lua_touserdata(L, 1);
	int size = luaL_checkinteger(L, 2);

	MemoryStream ms(data, size);

	std::string file;
	std::string log;

	ms >> file >> log;

	lua_pushlstring(L, file.c_str(), file.length());
	lua_pushlstring(L, log.c_str(), log.length());

	return 2;
}