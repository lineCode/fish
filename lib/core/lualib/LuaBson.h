#ifndef BSONLUA_H
#define BSONLUA_H

#include "bson.h"

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
};

class LuaBson
{
public:
	LuaBson(void);
	~LuaBson(void);

	static int	Encode(lua_State* L);
	static int  EncodeOrder(lua_State* L);
	static int	Decode(lua_State* L);
	
	static int	Register(lua_State* L);

private:
	static bson::BSONObj EncodeTable(lua_State* L);
	static void			 EncodeOne(bson::BSONObjBuilder& bob,lua_State* L,const char* key,size_t size);

	static void			 DecodeTable(lua_State* L,const char* data,int size);
};


#endif