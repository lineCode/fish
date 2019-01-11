#include "LuaUtil.h"
#include "ServerApp.h"
#include "util/MemoryStream.h"
#include "util/linenoise.hpp"

int LuaUtil::Register(lua_State* L) {
	luaL_checkversion(L);

	luaL_Reg methods[] = {
		{ "ParseLoggerMessage", LuaUtil::ParseLoggerMessage },
		{ "Readline", LuaUtil::Readline },
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

	int32_t type;
	std::string file;

	ms >> type >> file;

	lua_pushinteger(L, type);
	lua_pushlstring(L, file.c_str(), file.length());

	int numArgs;
	if (type == 0) {
		std::string log;
		ms >> log;

		lua_pushlstring(L, log.c_str(), log.length());
		numArgs = 3;
	}  else {
		size_t size;
		ms >> size;
		char* data = ms.Peek(size);

		lua_pushlightuserdata(L, data);
		lua_pushinteger(L, size);
		numArgs = 4;
	}

	return 3;
}

static lua_State* gL = NULL;

void CompletionCallback(const char* buffer, std::vector<std::string>& completions) {

	lua_pushvalue(gL, 3);
	lua_pushstring(gL, buffer);
	int r = lua_pcall(gL, 1, 1, 0);
	if ( r != LUA_OK )  {
		fprintf(stderr, "%s\n", lua_tostring(gL, -1));
		lua_pop(gL, 1);
		return;
	}

	if ( !lua_isnil(gL, -1) ) {
		lua_pushnil(gL);
		while ( lua_next(gL, -2) != 0 ) {
			const char* result = lua_tostring(gL, -1);
			completions.push_back(result);
			lua_pop(gL, 1);
		}
	}
	lua_pop(gL, 1);
}


int LuaUtil::Readline(lua_State* L) {
	const char* prompt = luaL_optstring(L, 1, ">>");
	const char* history = luaL_optstring(L, 2, "history.txt");

	linenoise::SetMultiLine(true);
	linenoise::SetHistoryMaxLen(10);
	linenoise::SetCompletionCallback(CompletionCallback);
	linenoise::LoadHistory(history);

	gL = L;
	std::string line = linenoise::Readline(prompt);
	if ( line.empty() == false ) {
		linenoise::AddHistory(line.c_str());
		linenoise::SaveHistory(history);

		gL = NULL;
		lua_pushstring(L, line.c_str());
		return 1;
	}
	gL = NULL;
	return 0;
}
