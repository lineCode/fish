#include "LuaServer.h"
#include "../network/TcpReader.h"

LuaServer::LuaServer(ServerApp* app,Network::EventPoller * poller,int fd):
	Session(poller,fd),_app(app)
{
}


LuaServer::~LuaServer(void)
{
}

int LuaServer::Init()
{
	_app->SessionEnter(_fd,this);
	return _app->LuaManager()->DispatchServerEvent(_fd,true);
}

int LuaServer::Fina()
{
	_app->SessionLeave(_fd);
	return _app->LuaManager()->DispatchServerEvent(_fd,false);
}

int LuaServer::Close()
{
	_state = Session::Closed;

	if (_sendQueue.empty())
		this->HandleError();
	return 0;
}

int LuaServer::Forward(const char * ptr,int size)
{
	return _app->LuaManager()->DispatchServer(_fd,ptr,size);
}

int LuaServer::Register(lua_State* L)
{
	luaL_checkversion(L);

	luaL_Reg l[] =
	{
		{ "New" ,LuaServer::_New },
		{ "Delete", LuaServer::_Delete },
		{ "Close", LuaServer::_Close },
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
int LuaServer::_New(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

	int fd = lua_tointeger(L,1);
	int buffsize = lua_tointeger(L,2);
	int id = app->Poller()->GenId(fd);
	if (id == 0)
	{
		Network::SocketClose(fd);
		lua_pushboolean(L,0);
		lua_pushstring(L,"error socket max limited");
		return 2;
	}

	LuaServer* self = new LuaServer(app,app->Poller(),fd);

	self->SetId(id);
	self->SetReader(new Network::TcpReader(self,2,buffsize));

	app->Poller()->RegisterRead(id,fd,self);
	app->Poller()->RegisterError(id,self);

	self->Init();

	lua_pushlightuserdata(L,self);
	return 1;
}

int LuaServer::_Delete(lua_State* L)
{
	LuaServer* self = (LuaServer*)lua_touserdata(L,1);
	delete self;
	return 0;
}

int LuaServer::_Close(lua_State* L)
{
	LuaServer* self = (LuaServer*)lua_touserdata(L,1);

	if (self->IsAlive() == false)
		luaL_error(L,"socket:%d not alive",self->_fd);

	self->Close();
	return 0;
}