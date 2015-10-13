#include "LuaBson.h"
#include "../util/MemoryStream.h"
#define LUA_TSHRSTR	(LUA_TSTRING | (0 << 4))

LuaBson::LuaBson(void)
{
}


LuaBson::~LuaBson(void)
{
}

void LuaBson::EncodeOne(bson::BSONObjBuilder& bob,lua_State* L,const char* key,size_t size)
{
	int vt = lua_type(L,-1);
	switch(vt) 
	{
	case LUA_TNUMBER:
		{
			int64 i = lua_tointeger(L, -1);
			lua_Number d = lua_tonumber(L,-1);
			if (i != d)
				bob.append(bson::StringData(key,size),(double)d); 
			else 
			{
				int si = (int)i >> 31;
				if (si == 0 || si == -1)
					bob.append(bson::StringData(key,size),(int)i);
				else
					bob.append(bson::StringData(key,size),(long long)i);
			}
		}
		break;
	case LUA_TUSERDATA: 
		{
			luaL_error(L,"EncodeOne::error userdata");
		}
		break;	
	case LUA_TSTRING:
		{
			size_t len;
			const char * str = lua_tolstring(L,-1,&len);
			bob.append(bson::StringData(key,size),str,len+1);
		}
		break;
	case LUA_TTABLE:
		{
			bson::BSONObj bo = LuaBson::EncodeTable(L);
			bob.appendObject(bson::StringData(key,size),bo.objdata(),bo.objsize());
		}

		break;
	case LUA_TBOOLEAN:
		{
			bob.append(bson::StringData(key,size),(bool)lua_toboolean(L,-1));
		}
		break;
	default:
		luaL_error(L, "Invalid value type : %s", lua_typename(L,vt));
	}
}

bson::BSONObj LuaBson::EncodeTable(lua_State* L)
{
	bson::BSONObjBuilder bob;
	lua_pushnil(L);
	while(lua_next(L,-2) != 0) 
	{
		int kt = lua_type(L, -2);
		switch(kt)
		{
		case LUA_TNUMBER:
			{
				int32 x = (int32)lua_tointeger(L,-2);
				lua_Number n = lua_tonumber(L,-2);
				if ((lua_Number)x!=n) 
					luaL_error(L,"unsuport key double");

				lua_pushvalue(L,-2);
				lua_insert(L,-2);
				size_t sz;
				const char* key = lua_tolstring(L,-2,&sz);
				LuaBson::EncodeOne(bob,L, key, sz);
				lua_pop(L,2);
				break;
			}
		case LUA_TSHRSTR:
			{
				size_t sz;
				const char *key = lua_tolstring(L,-2,&sz);
				LuaBson::EncodeOne(bob,L, key, sz);
				lua_pop(L,1);
				break;
			}
		}
	}
	return bob.obj();
}

void LuaBson::DecodeTable(lua_State* L,const char* data,int size)
{
	bson::BSONObj bo(data);
	std::vector<bson::BSONElement> elts;
	bo.elems(elts);

	lua_createtable(L,0,0);
	std::vector<bson::BSONElement>::iterator iter = elts.begin();
	for (;iter != elts.end();iter++)
	{
		lua_pushstring(L,iter->fieldName());
		switch(iter->type())
		{
		case bson::Bool:
			{
				lua_pushboolean(L,iter->Bool());
				break;
			}
		case bson::NumberInt:
			{
				lua_pushinteger(L,iter->Int());
				break;
			}

		case bson::NumberLong:
			{
				lua_pushnumber(L,iter->Long());
				break;
			}
		case bson::NumberDouble:
			{
				lua_pushnumber(L,iter->Double());
				break;
			}
		case bson::String:
			{
				lua_pushlstring(L,iter->valuestrsafe(),iter->valuestrsize());
				break;
			}
		case bson::Object:
			{
				DecodeTable(L,iter->value(),iter->valuesize());
				break;
			}
		default:
			{
				//luaL_error(L,"unknow support type:%d",iter->type());
				//printf("skip field:%s\n",iter->fieldName());
				lua_pop(L,1);
				continue;
			}
		}
		lua_rawset(L,-3);
	}
	return;
}

int LuaBson::Encode(lua_State* L)
{
	luaL_checktype(L,1,LUA_TTABLE);
	bson::BSONObj bo = LuaBson::EncodeTable(L);

	void * data = malloc(bo.objsize());
	memcpy(data,bo.objdata(),bo.objsize());
	lua_pushlightuserdata(L,data);
	lua_pushinteger(L,bo.objsize());
	return 2;
}

int LuaBson::EncodeOrder(lua_State* L)
{
	int n = lua_gettop(L);
	if (n%2 != 0)
		return luaL_error(L, "Invalid ordered dict");

	bson::BSONObjBuilder bob;

	for (int i=0;i<n;i+=2)
	{
		size_t sz;
		const char * key = lua_tolstring(L, i+1, &sz);
		if (key == NULL) {
			luaL_error(L, "Argument %d need a string", i+1);
		}
		lua_pushvalue(L, i+2);
		
		LuaBson::EncodeOne(bob,L,key,sz);
		lua_pop(L,1);
	}

	bson::BSONObj bo = bob.obj();
	void * data = malloc(bo.objsize());
	memcpy(data,bo.objdata(),bo.objsize());
	lua_pushlightuserdata(L,data);
	lua_pushinteger(L,bo.objsize());
	return 2;
}

int LuaBson::Decode(lua_State* L)
{
	char* data;
	size_t size;
	bool needFree = false;
	if (lua_isuserdata(L,1))
	{
		data = (char*)lua_touserdata(L,1);
		size = lua_tointeger(L,2);
		needFree = true;
	}
	else
		data = (char*)lua_tolstring(L,1,&size);

	LuaBson::DecodeTable(L,data,size);

	if (needFree)
		free((void*)data);
	
	return 1;
}


int LuaBson::Register(lua_State* L)
{
	luaL_checkversion(L);

	luaL_Reg l[] =
	{
		{ "Encode" , LuaBson::Encode },
		{ "EncodeOrder",LuaBson::EncodeOrder },
		{ "Decode", LuaBson::Decode },
		{ NULL, NULL },
	};

	lua_createtable(L, 0, (sizeof(l)) / sizeof(luaL_Reg) - 1);
	luaL_setfuncs(L, l, 0);

	return 1;
}
