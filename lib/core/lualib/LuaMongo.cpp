#include "LuaMongo.h"
#include "../ServerApp.h"

#include <stdlib.h>

namespace LuaMongo
{
	int _RunCommand(lua_State *L)
	{
	/*	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
		MongoBase* mongo = (MongoBase*)app->Mongo();
		if (mongo == NULL)
			luaL_error(L,"mongo not init!");

		if (mongo->IsAlive() == false)
			luaL_error(L,"mongo not connected!");

		size_t nsize;
		const char* name = lua_tolstring(L,1,&nsize);
		const char* cmd = (const char*)lua_touserdata(L,2);

		int session = mongo->doRunCommand(name,nsize,cmd,MongoCursor::BsonSize(cmd));

		lua_pushinteger(L,session);*/
		return 1;
	}

	int _Query(lua_State *L)
	{
	/*	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
		MongoBase* mongo = (MongoBase*)app->Mongo();
		if (mongo == NULL)
			luaL_error(L,"mongo not init!");
		if (mongo->IsAlive() == false)
			luaL_error(L,"mongo not connected!");

		size_t nsize;
		const char* name = lua_tolstring(L,1,&nsize);
		const char* query = (const char*)lua_touserdata(L,2);
		const char* selector = (const char*)lua_touserdata(L,3);
		int flag = lua_tointeger(L,4);
		int skip = lua_tointeger(L,5);
		int number = lua_tointeger(L,6);

		int session = mongo->doQuery(name,nsize,query,MongoCursor::BsonSize(query),selector,MongoCursor::BsonSize(selector),flag,skip,number);

		lua_pushinteger(L,session);*/
		return 1;
	}

	int _More(lua_State* L)
	{
		/*ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
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

		lua_pushinteger(L,session);*/
		return 1;
	}

	int _Update(lua_State *L)
	{
	/*	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
		MongoBase* mongo = (MongoBase*)app->Mongo();
		if (mongo == NULL)
			luaL_error(L,"mongo not init!");

		if (mongo->IsAlive() == false)
			luaL_error(L,"mongo not connected!");

		size_t nsize;
		const char* name = lua_tolstring(L,1,&nsize);
		int flag = lua_tointeger(L,2);
		const char* selector = (const char*)lua_touserdata(L,3);
		const char* updator = (const char*)lua_touserdata(L,4);

		mongo->doUpdate(name,nsize,flag,selector,MongoCursor::BsonSize(selector),updator,MongoCursor::BsonSize(updator));
*/
		return 0;
	}

	int _Insert(lua_State *L)
	{
		/*ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
		MongoBase* mongo = (MongoBase*)app->Mongo();
		if (mongo == NULL)
			luaL_error(L,"mongo not init!");

		if (mongo->IsAlive() == false)
			luaL_error(L,"mongo not connected!");

		size_t nsize;
		const char* name = lua_tolstring(L,1,&nsize);
		int flag = lua_tointeger(L,2);
		const char* document = (const char*)lua_touserdata(L,3);

		mongo->doInsert(name,nsize,flag,document,MongoCursor::BsonSize(document));*/

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