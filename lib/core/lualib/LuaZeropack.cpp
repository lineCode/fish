#include "LuaZeropack.h"
#include "../util/ZeroPack.h"

#define ENCODE_BUFFERSIZE 2050
#define ENCODE_MAXSIZE 0x1000000

namespace LuaZeropack
{
	static void * GetBuffer(lua_State *L, int index, size_t *sz)
	{
		void * buffer = NULL;
		int t = lua_type(L, index);
		if (t == LUA_TSTRING)
			buffer = (void*)lua_tolstring(L, index, sz);
		else
		{
			if (t != LUA_TUSERDATA && t != LUA_TLIGHTUSERDATA) 
			{
				luaL_argerror(L, index, "Need a string or userdata");
				return NULL;
			}
			buffer = lua_touserdata(L, index);
			*sz = luaL_checkinteger(L, index+1);
		}
		return buffer;
	}

	static void * ExpandBuffer(lua_State *L, int osz, int nsz)
	{
		void *output;
		do 
		{
			osz *= 2;
		} while (osz < nsz);
		if (osz > ENCODE_MAXSIZE)
		{
			luaL_error(L, "object is too large (>%d)", ENCODE_MAXSIZE);
			return NULL;
		}

		output = lua_newuserdata(L, osz);
		lua_replace(L, lua_upvalueindex(1));
		lua_pushinteger(L, osz);
		lua_replace(L, lua_upvalueindex(2));

		return output;
	}

	int _Pack(lua_State* L)
	{
		size_t sz = 0;
		void * buffer = GetBuffer(L, 1, &sz);

		size_t maxsz = (sz + 2047) / 2048 * 2 + sz;
		void * output = lua_touserdata(L, lua_upvalueindex(1));
		int bytes;
		int osz = lua_tointeger(L, lua_upvalueindex(2));
		if (osz < maxsz)
			output = ExpandBuffer(L, osz, maxsz);
		
		bytes = Zeropack(buffer, sz, output, maxsz);
		if (bytes > maxsz)
			return luaL_error(L, "packing error, return size = %d", bytes);
		
		lua_pushlstring(L, (const char*)output, bytes);

		return 1;
	}

	int _Unpack(lua_State* L)
	{
		size_t sz = 0;
		void * buffer = GetBuffer(L, 1, &sz);
		void * output = lua_touserdata(L, lua_upvalueindex(1));
		int osz = lua_tointeger(L, lua_upvalueindex(2));
		int r = Zerounpack(buffer, sz, output, &osz);
		if (r < 0)
			return luaL_error(L, "Invalid unpack stream");
		if (r > osz) {
			output = ExpandBuffer(L, osz, r);
			r = Zerounpack(buffer, sz, output, &r);
			if (r < 0)
				return luaL_error(L, "Invalid unpack stream");
		}
		
		lua_pushlstring(L, (const char*)output, r);
		return 1;
	}

	int Register(lua_State* L)
	{
		luaL_checkversion(L);

		luaL_Reg l[] =
		{
			{ "Pack", _Pack },
			{ "Unpack", _Unpack },
			{ NULL, NULL },
		};

		luaL_newlibtable(L, l);
	
		lua_newuserdata(L, ENCODE_BUFFERSIZE);
		lua_pushinteger(L, ENCODE_BUFFERSIZE);

		luaL_setfuncs(L,l,2);

		return 1;
	}
}