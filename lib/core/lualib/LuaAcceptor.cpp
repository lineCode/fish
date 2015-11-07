#include "LuaAcceptor.h"

LuaAcceptor::LuaAcceptor(ServerApp* app,Network::EventPoller* poller):
	_app(app),
	_poller(poller),
	_host(""),
	_port(-1),
	_fd(-1),
	_id(-1),
	_listening(false)
{
}

LuaAcceptor::~LuaAcceptor(void)
{
}

int LuaAcceptor::Listen(const char * host,int port)
{
	_host.assign(host);
	_port = port;

	int fd = Network::SocketBind(_host.c_str(),_port,IPPROTO_TCP);
	if (Network::SocketListen(fd,32) == -1) 
		return -1;

	_fd = fd;
	int id = _poller->GenId(_fd);
	if (id == 0)
	{
		//socket count max
		Network::SocketClose(_fd);
		return -1;
	}

	_id = id;
	_listening = true;

	this->_poller->RegisterRead(id,fd,this);
	return _fd;
}

int LuaAcceptor::HandleInput()
{
	struct sockaddr	sa;
	int len = sizeof(sa);
	int rfd = accept(_fd, (sockaddr*)&sa, (socklen_t*)&len);

	Network::SocketSetKeepalive(rfd,true);
	Network::SocketSetNodelay(rfd,true);
	Network::SocketSetNonblocking(rfd,true);

	this->_app->LuaManager()->DispatchSocketEvent(_fd,LuaFish::Accept,rfd);

	return 0;
}

int LuaAcceptor::Register(lua_State* L)
{
	luaL_checkversion(L);

	luaL_Reg l[] =
	{
		{ "New" ,LuaAcceptor::_New },
		{ "Delete", LuaAcceptor::_Delete },
		{ "Listen", LuaAcceptor::_Listen },
		{ "Close", LuaAcceptor::_Close },
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

int LuaAcceptor::_New(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
	LuaAcceptor* self = new LuaAcceptor(app,app->Poller());
	lua_pushlightuserdata(L,self);
	return 1;
}

int LuaAcceptor::_Delete(lua_State* L)
{
	LuaAcceptor* self = (LuaAcceptor*)lua_touserdata(L,1);
	delete self;
	return 0;
}

int LuaAcceptor::_Listen(lua_State* L)
{
	LuaAcceptor* self = (LuaAcceptor*)lua_touserdata(L,1);
	size_t size;
	const char* host = lua_tolstring(L,2,&size);
	int port = lua_tointeger(L,3);
	int fd = self->Listen(host,port);
	lua_pushinteger(L,fd);
	return 1;
}

int LuaAcceptor::_Close(lua_State* L)
{
	LuaAcceptor* self = (LuaAcceptor*)lua_touserdata(L,1);

	if (self->_listening == false)
		luaL_error(L,"LuaAcceptor already closed!");
	
	if (self->_poller->isRegistered(self->_id,true))
		self->_poller->DeRegisterRead(self->_id,self->_fd);

	Network::SocketClose(self->_fd);

	self->_poller->RetrieveId(self->_fd,self->_id);

	self->_listening = false;

	return 0;
}
