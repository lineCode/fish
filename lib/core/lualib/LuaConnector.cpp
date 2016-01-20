#include "LuaConnector.h"
#include <assert.h>

LuaConnector::LuaConnector(ServerApp* app,Network::EventPoller* poller):
	_app(app),
	_poller(poller),
	_host(""),
	_port(-1),
	_fd(-1),
	_id(-1)
{
}

LuaConnector::~LuaConnector(void)
{
}

int LuaConnector::Connect(const char * host,int port,bool& done)
{
	_host.assign(host);
	_port = port;

	bool connected = false;

	int fd = Network::SocketConnect(host,port,false,connected);
	if (fd < 0)
		return -1;

	_fd = fd;

	int id = _poller->GenId(_fd);
	if (id == 0)
	{
		//socket count max
		_poller->RetrieveId(_fd,id);
		Network::SocketClose(_fd);
		return -1;
	}

	_id = id;

	if (connected)
	{
		_poller->RetrieveId(_fd,_id);
		done = true;
		return _fd;
	}
	else
	{
		_poller->RegisterError(_id,_fd,this);
		_poller->RegisterWrite(_id,_fd,this);
	}

	done = false;
	return _fd;
}

int LuaConnector::HandleOutput()
{
	_poller->DeRegisterWrite(_id,_fd);
	_poller->DeRegisterError(_id,_fd);
	_poller->RetrieveId(_fd,_id);

	if (Network::SocketHasError(_fd))
	{
		Network::SocketClose(_fd);
		this->_app->LuaManager()->DispatchSocketEvent(_fd,LuaFish::Connect,0);
	}
	else
		this->_app->LuaManager()->DispatchSocketEvent(_fd,LuaFish::Connect,1);
	return 0;
}

int LuaConnector::HandleError()
{
	_poller->DeRegisterWrite(_id,_fd);
	_poller->DeRegisterError(_id,_fd);
	_poller->RetrieveId(_fd,_id);
	Network::SocketClose(_fd);
	this->_app->LuaManager()->DispatchSocketEvent(_fd,LuaFish::Connect,0);
	return 0;
}

int LuaConnector::Register(lua_State* L)
{
	luaL_checkversion(L);

	luaL_Reg l[] =
	{
		{ "New" ,LuaConnector::_New },
		{ "Delete", LuaConnector::_Delete },
		{ "Connect", LuaConnector::_Connect },
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

int LuaConnector::_New(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	LuaConnector* self = new LuaConnector(app,app->Poller());
	lua_pushlightuserdata(L,self);
	return 1;
}

int LuaConnector::_Delete(lua_State* L)
{
	LuaConnector* self = (LuaConnector*)lua_touserdata(L,1);
	delete self;
	return 0;
}

int LuaConnector::_Connect(lua_State* L)
{
	LuaConnector* self = (LuaConnector*)lua_touserdata(L,1);

	size_t size = 0;
	const char* host = lua_tolstring(L,2,&size);
	int port = lua_tointeger(L,3);

	bool done = false;
	int fd;
	if ((fd = self->Connect(host,port,done)) < 0)
	{
		lua_pushboolean(L,0);
		lua_pushstring(L,"error socket max limited");
		return 2;
	}

	lua_pushboolean(L,1);
	lua_pushboolean(L,(int)done);
	lua_pushinteger(L,fd);
	return 3;
}
