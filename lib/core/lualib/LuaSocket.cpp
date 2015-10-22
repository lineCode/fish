#include "LuaSocket.h"
#include "LuaReader.h"

LuaSocket::LuaSocket(ServerApp* app,Network::EventPoller * poller,int fd):
	Session(poller,fd),_app(app),_total(0),_waitlist(),_freelist()
{
}


LuaSocket::~LuaSocket(void)
{
	while (_freelist.Empty() == false)
	{
		MemoryStream* ms;
		_freelist.PopHead(ms);
		delete ms;
	}

	while(_waitlist.Empty() == false)
	{
		MemoryStream* ms;
		_waitlist.PopHead(ms);
		delete ms;
	}
	delete _reader;
}

int LuaSocket::Init()
{
	return 0;
}

int LuaSocket::Fina()
{
	return _app->LuaManager()->DispatchSocketEvent(_fd,LuaFish::Close);
}

int LuaSocket::Close()
{
	_state = Session::Closed;

	if (_sendQueue.empty())
		return this->Clean();

	return -1;
}

int LuaSocket::Forward(const char * ptr,int size)
{
	return _app->LuaManager()->DispatchSocket(_fd,ptr,size);
}

void LuaSocket::PopStream(MemoryStream*& ms)
{
	ms = NULL;
	if (_waitlist.Empty() == false)
	{
		ms = _waitlist.Back();
		if ((int)ms->size() - ms->wpos() > 0)
			return;
	}
	
	if (_freelist.Empty())
		ms = new MemoryStream();
	else
	{
		ms = _freelist.Front();
		_freelist.RemoveFront();
	}
	_waitlist.PushTail(ms);
	
	return;
}

void LuaSocket::PushStream(MemoryStream* ms)
{
	ms->reset();
	_freelist.PushTail(ms);
}

int LuaSocket::Register(lua_State* L)
{
	luaL_checkversion(L);

	luaL_Reg l[] =
	{
		{ "New" ,LuaSocket::_New },
		{ "Delete", LuaSocket::_Delete },
		{ "SaveBuffer", LuaSocket::_SaveBuffer },
		{ "ReadBuffer", LuaSocket::_ReadBuffer },
		{ "Send", LuaSocket::_Send },
		{ "Close", LuaSocket::_Close },
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
int LuaSocket::_New(lua_State* L)
{
	ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

	int fd = lua_tointeger(L,1);
	int buffersize = lua_tointeger(L,2);
	int id = app->Poller()->GenId(fd);
	if (id == 0)
	{
		//socket count max
		Network::SocketClose(fd);
		lua_pushboolean(L,0);
		lua_pushstring(L,"error socket max limited");
		return 2;
	}

	LuaSocket* self = new LuaSocket(app,app->Poller(),fd);

	self->SetId(id);
	self->SetReader(new LuaReader(self,buffersize));
	
	app->Poller()->RegisterRead(id,fd,self);
	app->Poller()->RegisterError(id,self);

	self->Init();

	lua_pushlightuserdata(L,self);
	return 1;
}

int LuaSocket::_Delete(lua_State* L)
{
	LuaSocket* self = (LuaSocket*)lua_touserdata(L,1);
	delete self;
	return 0;
}

int LuaSocket::_SaveBuffer(lua_State* L)
{
	LuaSocket* self = (LuaSocket*)lua_touserdata(L,1);
	const char* buffer = (const char*)lua_touserdata(L,2);
	void* obuffer = (void*)buffer;
	int size = lua_tointeger(L,3);

	self->_total += size;

	MemoryStream* ms = NULL;

	int left = size;
	while (left > 0)
	{
		self->PopStream(ms);
		int length = (int)ms->size() - ms->wpos();
		if (left < length)
		{
			ms->append(buffer,left);
			break;
		}
		else
		{
			ms->append(buffer,length);
			buffer += length;
			left -= length;
		}
	}
	free((void*)obuffer);
	lua_pushinteger(L,self->_total);
	return 1;
}

int LuaSocket::_ReadBuffer(lua_State* L)
{
	LuaSocket* self = (LuaSocket*)lua_touserdata(L,1);
	int size = lua_tointeger(L,2);
	
	assert(self->_total >= size);

	luaL_Buffer b;
	luaL_buffinit(L, &b);

	while (size > 0)
	{
		assert(self->_total > 0);
		assert(self->_waitlist.Empty() == false);

		MemoryStream* ms = self->_waitlist.Front();
		if ((int)ms->length() >= size)
		{
			luaL_addlstring(&b, ms->begin(), size);
			ms->rpos(ms->rpos() + size);
			self->_total -= size;
			break;
		}
		else
		{
			luaL_addlstring(&b, ms->begin(), ms->length());
			self->_total -=  ms->length();
			size -= ms->length();
			ms->rpos(ms->rpos() + ms->length());
			self->_waitlist.RemoveFront();
			self->PushStream(ms);
		}
	}
	
	luaL_pushresult(&b);
	return 1;
}

int LuaSocket::_Send(lua_State* L)
{
	LuaSocket* self = (LuaSocket*)lua_touserdata(L,1);

	char* data;
	size_t size;
	bool needFree = false;
	if (lua_isuserdata(L,2))
	{
		data = (char*)lua_touserdata(L,2);
		size = lua_tointeger(L,3);
		needFree = true;
	}
	else
		data = (char*)lua_tolstring(L,2,&size);

	int result = self->Send((char*)data,size);

	if (needFree)
		free((void*)data);
	
	lua_pushboolean(L,result == 0);
	return 1;
}

int LuaSocket::_Close(lua_State* L)
{
	LuaSocket* self = (LuaSocket*)lua_touserdata(L,1);

	if (self->IsAlive() == false)
		luaL_error(L,"socket:%d not alive!",self->_fd);

	int done = 1;
	if (self->Close() < 0)
		done = 0;

	lua_pushboolean(L,done);
	return 1;
}