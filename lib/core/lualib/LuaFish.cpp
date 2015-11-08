#include "LuaFish.h"
#include "../ServerApp.h"
#include "../Logger.h"
#include "../util/MemoryStream.h"
#include "../util/BufferHelper.h"
#include "../mongo/MongoCursor.h"
#include "../time/Timestamp.h"

#define DISPATCH_TYPE_CLIENT	0
#define DISPATCH_TYPE_SESSION	1
#define DISPATCH_TYPE_SOCKET	2
#define DISPATCH_TYPE_MONGO		3
#define DISPATCH_TYPE_TIMER		4

#define SESSION_TYPE_DATA		1
#define SESSION_TYPE_EVENT		2

LuaFish::LuaFish(void)
{
	_L = luaL_newstate();
	_sessionCounter = 0;
	_callback = -1;
	_mainTick = -1;
}


LuaFish::~LuaFish(void)
{
	lua_close(_L);
}

lua_State* LuaFish::LuaState()
{
	return _L;
}

void LuaFish::Require(const char* module,int (*func)(lua_State*))
{
	luaL_requiref(_L,module,func,0);
}

int LuaFish::Init(ServerApp* app)
{
	luaL_openlibs(_L);

	lua_pushlightuserdata(_L,app);
	lua_setfield(_L, LUA_REGISTRYINDEX, "CoreCtx");
	return 0;
}

int LuaFish::DoFile(const char* file)
{
	int r = luaL_loadfile(_L,file);
	if (r != LUA_OK) 
	{
		fprintf(stderr,"%s\n",lua_tostring(_L,-1));
		return -1;
	}

	r = lua_pcall(_L,0,0,0);
	if (r != LUA_OK) 
	{
		fprintf(stderr,"%s\n",lua_tostring(_L,-1));
		return -1;
	}
	return 0;
}

int LuaFish::MainTick()
{
	int otop = lua_gettop(_L);

	lua_pushcfunction(_L, _Traceback);
	lua_rawgeti(_L, LUA_REGISTRYINDEX, _mainTick);

	int r = lua_pcall(_L,0,0,1);
	if (r != LUA_OK)
		Logger::GetSingleton().LuaLog(lua_tostring(_L,-1));

	lua_settop(_L,otop);
	return 0;
}

int LuaFish::DispatchClient(int source,int method,const char* data,int size)
{
	//data free by caller
	int otop = lua_gettop(_L);

	lua_pushcfunction(_L, _Traceback);
	lua_rawgeti(_L, LUA_REGISTRYINDEX, _callback);
	lua_pushinteger(_L,DISPATCH_TYPE_CLIENT);
	lua_pushinteger(_L,source);
	lua_pushboolean(_L,0);
	lua_pushinteger(_L,0);
	lua_pushinteger(_L,method);

	int argc = 5;
	if (data != NULL)
	{
		lua_pushlstring(_L,data,size);
		argc = 6;
	}
	
	int r = lua_pcall(_L,argc,0,1);
	if (r != LUA_OK)
		Logger::GetSingleton().LuaLog(lua_tostring(_L,-1));

	lua_settop(_L,otop);
	return 0;
}

int LuaFish::DispatchServerEvent(int source,bool start /* = true */)
{
	int otop = lua_gettop(_L);

	lua_pushcfunction(_L, _Traceback);
	lua_rawgeti(_L, LUA_REGISTRYINDEX, _callback);
	lua_pushinteger(_L,DISPATCH_TYPE_SESSION);
	lua_pushinteger(_L,source);
	lua_pushboolean(_L,0);
	lua_pushinteger(_L,0);
	lua_pushinteger(_L,SESSION_TYPE_EVENT);
	lua_pushboolean(_L,start);

	int r = lua_pcall(_L,6,0,1);
	if (r != LUA_OK)
		Logger::GetSingleton().LuaLog(lua_tostring(_L,-1));
	
	lua_settop(_L,otop);
	return 0;
}

int LuaFish::DispatchServer(int source,const char* data,int size)
{
	int otop = lua_gettop(_L);

	lua_pushcfunction(_L, _Traceback);
	lua_rawgeti(_L, LUA_REGISTRYINDEX, _callback);
	lua_pushinteger(_L,DISPATCH_TYPE_SESSION);
	lua_pushinteger(_L,source);

	BufferHelper helper((char*)data,size);
	bool response;
	short session = 0;
	short method = 0;
	helper >> response >> session >> method;

	lua_pushboolean(_L,response);
	lua_pushinteger(_L,session);
	lua_pushinteger(_L,SESSION_TYPE_DATA);
	lua_pushinteger(_L,method);
	lua_pushlstring(_L,helper.begin(),helper.length());
	
	free((void*)data);

	int r = lua_pcall(_L,7,0,1);
	if (r != LUA_OK)
		Logger::GetSingleton().LuaLog(lua_tostring(_L,-1));
	lua_settop(_L,otop);
	return 0;
}

int LuaFish::DispatchSocketEvent(int source,SocketEvent ev,int reserve /* = 0 */)
{
	int otop = lua_gettop(_L);

	lua_pushcfunction(_L, _Traceback);
	lua_rawgeti(_L, LUA_REGISTRYINDEX, _callback);
	lua_pushinteger(_L,DISPATCH_TYPE_SOCKET);
	lua_pushinteger(_L,source);
	lua_pushboolean(_L,0);
	lua_pushinteger(_L,0);
	lua_pushinteger(_L,(int)ev);
	
	int nargs = 5;
	if (ev == Accept || ev == Connect)
	{
		nargs = 6;
		lua_pushinteger(_L,reserve);
	}

	int r = lua_pcall(_L,nargs,0,1);
	if (r != LUA_OK)
		Logger::GetSingleton().LuaLog(lua_tostring(_L,-1));

	lua_settop(_L,otop);
	return 0;
}

int LuaFish::DispatchSocket(int source,const char* data,int size)
{
	int otop = lua_gettop(_L);

	lua_pushcfunction(_L, _Traceback);
	lua_rawgeti(_L, LUA_REGISTRYINDEX, _callback);
	lua_pushinteger(_L,DISPATCH_TYPE_SOCKET);
	lua_pushinteger(_L,source);
	lua_pushboolean(_L,0);
	lua_pushinteger(_L,0);
	lua_pushinteger(_L,(int)Data);
	//free by LuaSession
	lua_pushlightuserdata(_L,(void*)data);
	lua_pushinteger(_L,size);

	int r = lua_pcall(_L,7,0,1);
	if (r != LUA_OK)
		Logger::GetSingleton().LuaLog(lua_tostring(_L,-1));
	lua_settop(_L,otop);
	return 0;
}

int LuaFish::DispatchMongo(int source,int session,const char* data,int size)
{
	int otop = lua_gettop(_L);

	//data free by caller
	lua_pushcfunction(_L, _Traceback);
	lua_rawgeti(_L, LUA_REGISTRYINDEX, _callback);
	lua_pushinteger(_L,DISPATCH_TYPE_MONGO);
	lua_pushinteger(_L,source);
	lua_pushboolean(_L,0);
	lua_pushinteger(_L,0);
	lua_pushinteger(_L,session);

	lua_newtable(_L);

	MongoCursor cursor(data,size);
	int index = 1;
	while (cursor.More())
	{
		char* data = cursor.Next();
		int size =  MongoCursor::BsonSize(data);
		lua_pushlightuserdata(_L,data);
		lua_rawseti(_L,-2,index++);
	}
	
	int r = lua_pcall(_L,6,0,1);
	if (r != LUA_OK)
		Logger::GetSingleton().LuaLog(lua_tostring(_L,-1));
	lua_settop(_L,otop);
	return 0;
}

int LuaFish::DispatchTimer(int session)
{
	int otop = lua_gettop(_L);

	lua_pushcfunction(_L, _Traceback);
	lua_rawgeti(_L, LUA_REGISTRYINDEX, _callback);
	lua_pushinteger(_L,DISPATCH_TYPE_TIMER);
	lua_pushinteger(_L,0);
	lua_pushboolean(_L,true);
	lua_pushinteger(_L,session);

	int r = lua_pcall(_L,4,0,1);
	if (r != LUA_OK)
		Logger::GetSingleton().LuaLog(lua_tostring(_L,-1));
	lua_settop(_L,otop);
	return 0;
}

int LuaFish::GenSession()
{
	return ++_sessionCounter;
}

void LuaFish::SetCallBack(int index)
{
	_callback = index;
}

void LuaFish::SetMainTick(int index)
{
	_mainTick = index;
}

void LuaFish::LuaPath(const char* npath)
{
	luaL_Buffer buffer;
	luaL_buffinit(_L, &buffer);

	lua_getglobal(_L,"package");
	lua_getfield(_L,-1,"path");

	size_t size = 0;
	const char* opath = lua_tolstring(_L,-1,&size);
	luaL_addlstring(&buffer,npath,strlen(npath));
	luaL_addlstring(&buffer,opath,size);

	lua_pop(_L,1);

	lua_pushlstring(_L,buffer.b,buffer.n);
	lua_setfield(_L,-2,"path");
}

int LuaFish::Register(lua_State* L)
{
	luaL_checkversion(L);

	luaL_Reg l[] =
	{
		{ "Send" ,LuaFish::_Send },
		{ "SendClient" ,LuaFish::_SendClient },
		{ "Log" ,LuaFish::_Log },
		{ "Now" ,LuaFish::_Now },
		{ "Timestamp",LuaFish::_Timestamp},
		{ "TimestampToSecond",LuaFish::_TimestampToSecond},
		{ "GenSession" ,LuaFish::_GenSession },
		{ "CallBack" ,LuaFish::_CallBack },
		{ "MainTick" ,LuaFish::_MainTick },
		{ "Stop" ,LuaFish::_Stop },
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

int LuaFish::_Traceback(lua_State *L)
{
	const char *msg = lua_tostring(L, 1);
	if (msg)
		luaL_traceback(L, L, msg, 1);
	else
		lua_pushliteral(L, "(no error message)");
	return 1;
}

int LuaFish::_Send(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

	int addr = lua_tointeger(L,1);

	Network::Session* session = app->FindSession(addr);
	if (session == NULL)
		luaL_error(L,"error session:%d not found",addr);

	bool reponse = (bool)lua_toboolean(L,2);
	uint16 sess = lua_tointeger(L,3);
	uint16 method = lua_tointeger(L,4);

	void *buffer = NULL;
	size_t size = 0;
	bool needFree = false;
	if (lua_isuserdata(L,5)) 
	{
		buffer = lua_touserdata(L,5);
		size = luaL_checkinteger(L,6);
		needFree = true;
	} 
	else 
		buffer =  (void*)lua_tolstring(L, 5, &size);

	uint16 total = sizeof(uint16) * 3 + sizeof(bool) + size;
	MemoryStream* ms = new MemoryStream(total);

	EndianConvert::Apply<uint16>(&total);

	(*ms) << total <<  reponse << sess << method;
	ms->append((const uint8*)buffer,size);

	if (needFree)
		free(buffer);

	int result = session->Send(ms);

	lua_pushboolean(L,result == 0);
	return 1;
}

int LuaFish::_SendClient(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

	int addr = lua_tointeger(L,1);

	Network::Session* session = app->FindSession(addr);
	if (session == NULL)
		luaL_error(L,"error session:%d not found",addr);

	uint16 method = (uint16)lua_tointeger(L,2);
	size_t messageSize;
	const char* message = lua_tolstring(L,3,&messageSize);


	return 0;
}

int LuaFish::_Log(lua_State* L)
{
	const char* log = lua_tostring(L,1);
	std::string str(log);
	Logger::GetSingleton().LuaLog(str);
	return 0;
}

int LuaFish::_Now(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushnumber(L,app->Now());
	return 1;
}

int LuaFish::_Timestamp(lua_State* L)
{
	uint64 now = TimeStamp();
	lua_pushnumber(L,now);
	return 1;
}

int LuaFish::_TimestampToSecond(lua_State* L)
{
	lua_Number ti = lua_tonumber(L,1);
	lua_pushnumber(L,ti/StampPersecond());
	return 1;
}

int LuaFish::_GenSession(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	lua_pushinteger(L,app->LuaManager()->GenSession());
	return 1;
}

int LuaFish::_CallBack(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	app->LuaManager()->SetCallBack(luaL_ref(L, LUA_REGISTRYINDEX));
	return 0;
}

int LuaFish::_MainTick(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	app->LuaManager()->SetMainTick(luaL_ref(L, LUA_REGISTRYINDEX));
	return 0;
}

int LuaFish::_Stop(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	app->Stop();
	return 0;
}