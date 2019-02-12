#include "lua.hpp"
#include <time.h>
#include <stdio.h>
#include "time/Timestamp.h"

#define NANOSEC 1000000000

static inline double GetTime() {
#ifdef USE_CLOCKTIME
	struct timespec ti;
	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ti);
	return (double)ti.tv_sec + (double)ti.tv_nsec / NANOSEC;
#else
	return ::GetTimeMillis() / 1000;
#endif
}

static inline double DiffTime(double start) {
	double now = GetTime();
	return now - start;
}

static int CountorStart(lua_State *L) {
	lua_pushthread(L);

	lua_pushvalue(L, 1);	// push coroutine
	lua_rawget(L, lua_upvalueindex(2));
	if (!lua_isnil(L, -1)) {
		return luaL_error(L, "coroutine %p start profile more than once", lua_topointer(L, 1));
	}
	lua_pushvalue(L, 1);	// push coroutine
	lua_pushnumber(L, 0);
	lua_rawset(L, lua_upvalueindex(2));

	lua_pushvalue(L, 1);	// push coroutine
	double ti = GetTime();

	lua_pushnumber(L, ti);
	lua_rawset(L, lua_upvalueindex(1));

	return 0;
}

static int CountorStop(lua_State *L) {
	lua_pushthread(L);
	lua_pushvalue(L, 1);	// push coroutine
	lua_rawget(L, lua_upvalueindex(1));
	if (lua_type(L, -1) != LUA_TNUMBER) {
		return luaL_error(L, "call profile.start() before profile.stop()");
	} 
	double ti = DiffTime(lua_tonumber(L, -1));
	lua_pushvalue(L, 1);	// push coroutine
	lua_rawget(L, lua_upvalueindex(2));
	double total_time = lua_tonumber(L, -1);

	lua_pushvalue(L, 1);	// push coroutine
	lua_pushnil(L);
	lua_rawset(L, lua_upvalueindex(1));

	lua_pushvalue(L, 1);	// push coroutine
	lua_pushnil(L);
	lua_rawset(L, lua_upvalueindex(2));

	total_time += ti;
	lua_pushnumber(L, total_time);

	return 1;
}

static int LuaResume(lua_State *L) {
	lua_pushvalue(L, 1);
	lua_pushvalue(L, -1);
	lua_rawget(L, lua_upvalueindex(2));
	if (lua_isnil(L, -1)) {		// check total time
		lua_pop(L,2);	// pop from coroutine
	} else {
		lua_pop(L,1);
		double ti = GetTime();

		lua_pushnumber(L, ti);
		lua_rawset(L, lua_upvalueindex(1));	// set start time
	}

	lua_CFunction coResume = lua_tocfunction(L, lua_upvalueindex(3));
	return coResume(L);
}

static int LuaYield(lua_State *L) {
	lua_pushthread(L);
	lua_pushvalue(L, -1);
	lua_rawget(L, lua_upvalueindex(2));	// check total time
	if (lua_isnil(L, -1)) {
		lua_pop(L,2);
	} else {
		double ti = lua_tonumber(L, -1);
		lua_pop(L,1);

		lua_pushvalue(L, -1);	// push coroutine
		lua_rawget(L, lua_upvalueindex(1));
		double starttime = lua_tonumber(L, -1);
		lua_pop(L,1);

		double diff = DiffTime(starttime);
		ti += diff;
		lua_pushvalue(L, -1);	// push coroutine
		lua_pushnumber(L, ti);
		lua_rawset(L, lua_upvalueindex(2));
		lua_pop(L, 1);	// pop coroutine
	}

	lua_CFunction coYield = lua_tocfunction(L, lua_upvalueindex(3));

	return coYield(L);
}

int luaopen_cocountor_core(lua_State *L) {
	luaL_checkversion(L);
	luaL_Reg l[] = {
		{ "start", CountorStart },
		{ "stop", CountorStop },
		{ "resume", LuaResume },
		{ "yield", LuaYield },
		{ NULL, NULL },
	};
	luaL_newlibtable(L,l);
	lua_newtable(L);	// table thread->start time
	lua_newtable(L);	// table thread->total time

	lua_newtable(L);	// weak table
	lua_pushliteral(L, "kv");
	lua_setfield(L, -2, "__mode");

	lua_pushvalue(L, -1);
	lua_setmetatable(L, -3); 
	lua_setmetatable(L, -3);

	lua_pushnil(L);	// cfunction (coroutine.resume or coroutine.yield)
	luaL_setfuncs(L,l,3);

	int libtable = lua_gettop(L);

	lua_getglobal(L, "coroutine");
	lua_getfield(L, -1, "resume");

	lua_CFunction co_resume = lua_tocfunction(L, -1);
	if (co_resume == NULL)
		return luaL_error(L, "Can't get coroutine.resume");
	lua_pop(L,1);

	lua_getfield(L, libtable, "resume");
	lua_pushcfunction(L, co_resume);
	lua_setupvalue(L, -2, 3);
	lua_pop(L,1);

	lua_getfield(L, -1, "yield");

	lua_CFunction co_yield = lua_tocfunction(L, -1);
	if (co_yield == NULL)
		return luaL_error(L, "Can't get coroutine.yield");
	lua_pop(L,1);

	lua_getfield(L, libtable, "yield");
	lua_pushcfunction(L, co_yield);
	lua_setupvalue(L, -2, 3);
	lua_pop(L,1);

	lua_settop(L, libtable);

	return 1;
}
