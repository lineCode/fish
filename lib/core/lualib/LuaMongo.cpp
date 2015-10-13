#include "LuaMongo.h"
#include "../ServerApp.h"
#include "../mongo/MongoBase.h"
#include <stdlib.h>

namespace LuaMongo
{
	int _RunCommand(lua_State *L)
	{
		ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
		MongoBase* mongo = (MongoBase*)app->Mongo();
		if (mongo == NULL)
			luaL_error(L,"mongo not init!");

		if (mongo->IsAlive() == false)
			luaL_error(L,"mongo not connected!");

		size_t nsize;
		const char* name = lua_tolstring(L,1,&nsize);

		const char* cmd = (const char*)lua_touserdata(L,2);
		int csize = lua_tointeger(L,3);

		int session = mongo->doRunCommand(name,nsize,cmd,csize);

		free((void*)cmd);

		lua_pushinteger(L,session);
		return 1;
	}

	int _Query(lua_State *L)
	{
		ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
		MongoBase* mongo = (MongoBase*)app->Mongo();
		if (mongo == NULL)
			luaL_error(L,"mongo not init!");

		if (mongo->IsAlive() == false)
			luaL_error(L,"mongo not connected!");

		size_t nsize;
		const char* name = lua_tolstring(L,1,&nsize);

		const char* query = (const char*)lua_touserdata(L,2);
		int qsize = lua_tointeger(L,3);

		const char* selector = (const char*)lua_touserdata(L,4);
		int ssize = lua_tointeger(L,5);

		int flag = lua_tointeger(L,6);
		int skip = lua_tointeger(L,7);
		int number = lua_tointeger(L,8);

		int session = mongo->doQuery(name,nsize,query,qsize,selector,ssize,flag,skip,number);

		free((void*)query);
		free((void*)selector);

		lua_pushinteger(L,session);
		return 1;
	}

	int _More(lua_State* L)
	{
		ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
		MongoBase* mongo = (MongoBase*)app->Mongo();
		if (mongo == NULL)
			luaL_error(L,"mongo not init!");

		if (mongo->IsAlive() == false)
			luaL_error(L,"mongo not connected!");

		size_t nsize;
		const char* name = lua_tolstring(L,1,&nsize);

		int number = lua_tointeger(L,2);

		size_t cursorsize;
		const char* curosr = lua_tolstring(L,3,&cursorsize);
		assert(cursorsize == 8);
		int session = mongo->doMore(name,nsize,number,curosr);

		lua_pushinteger(L,session);
		return 1;
	}

	int _Update(lua_State *L)
	{
		ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
		MongoBase* mongo = (MongoBase*)app->Mongo();
		if (mongo == NULL)
			luaL_error(L,"mongo not init!");

		if (mongo->IsAlive() == false)
			luaL_error(L,"mongo not connected!");

		size_t nsize;
		const char* name = lua_tolstring(L,1,&nsize);

		int flag = lua_tointeger(L,2);

		const char* selector = (const char*)lua_touserdata(L,3);
		int ssize = lua_tointeger(L,4);

		const char* updator = (const char*)lua_touserdata(L,5);
		int usize = lua_tointeger(L,6);

		mongo->doUpdate(name,nsize,flag,selector,ssize,updator,usize);

		free((void*)selector);
		free((void*)updator);

		return 0;
	}

	int _Insert(lua_State *L)
	{
		ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
		MongoBase* mongo = (MongoBase*)app->Mongo();
		if (mongo == NULL)
			luaL_error(L,"mongo not init!");

		if (mongo->IsAlive() == false)
			luaL_error(L,"mongo not connected!");

		size_t nsize;
		const char* name = lua_tolstring(L,1,&nsize);

		int flag = lua_tointeger(L,2);

		const char* document = (const char*)lua_touserdata(L,3);
		int dsize = lua_tointeger(L,4);

		mongo->doInsert(name,nsize,flag,document,dsize);

		free((void*)document);

		return 0;
	}

	int Register(lua_State* L)
	{
		luaL_checkversion(L);

		luaL_Reg l[] =
		{
			{ "RunCommand" ,_RunCommand },
			{ "Query", _Query },
			{ "More", _More },
			{ "Update", _Update },
			{ "Insert", _Insert },
			{ NULL, NULL },
		};

		luaL_newlibtable(L, l);

		lua_getfield(L, LUA_REGISTRYINDEX, "CoreCtx");
		ServerApp *app = (ServerApp*)lua_touserdata(L,-1);
		if (app == NULL)
			return luaL_error(L, "Init ServerApp context first");

		luaL_setfuncs(L,l,1);

		return 1;
	}
}