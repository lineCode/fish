#include "MessageReader.h"
#include <assert.h>
#include "../ServerApp.h"
#include "ZeroPack.h"

namespace MessageHelper
{
	MessageReader::MessageReader(char* buffer,int size):_data(buffer),_pos(0),_size(size)
	{
	}

	MessageReader::MessageReader():_data(NULL),_pos(0),_size(0)
	{

	}
	MessageReader::~MessageReader(void)
	{
	}

	MessageReader& MessageReader::operator>>(bool &value) 
	{
		uint8 type = read<uint8>();
		assert(type == TYPE_BOOL);
		value = read<bool>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(uint8 &value) 
	{
		uint8 type = read<uint8>();
		assert(type == TYPE_UINT8);
		value = read<uint8>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(uint16 &value) 
	{
		uint8 type = read<uint8>();
		assert(type == TYPE_UINT16);
		value = read<uint16>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(uint32 &value) 
	{
		uint8 type = read<uint8>();
		assert(type == TYPE_UINT32);
		value = read<uint32>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(uint64 &value)
	{
		uint8 type = read<uint8>();
		assert(type == TYPE_UINT64);
		value = read<bool>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(int8 &value) 
	{
		uint8 type = read<uint8>();
		assert(type == TYPE_INT8);
		value = read<int8>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(int16 &value)
	{
		uint8 type = read<uint8>();
		assert(type == TYPE_UINT16);
		value = read<int16>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(int32 &value)
	{
		uint8 type = read<uint8>();
		assert(type == TYPE_INT32);
		value = read<int32>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(int64 &value)
	{
		uint8 type = read<uint8>();
		assert(type == TYPE_INT64);
		value = read<int64>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(float &value)
	{
		uint8 type = read<uint8>();
		assert(type == TYPE_FLOAT);
		value = read<float>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(double &value) 
	{
		uint8 type = read<uint8>();
		assert(type == TYPE_DOUBLE);
		value = read<double>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(std::string& value)
	{
		value.clear();

		uint8 type = read<uint8>();
		assert(type == TYPE_STRING);
		uint16 len = read<uint16>();

		value.assign(&_data[_pos],len);
		_pos += len;
		return *this;
	}

	void MessageReader::SetBuffer(char* buffer,int size)
	{
		_data = buffer;
		_size = size;
		_pos = 0;
	}

	int MessageReader::Pos()
	{
		return _pos;
	}

	int MessageReader::Left()
	{
		return _size - _pos;
	}

	int MessageReader::Length()
	{
		return _size;
	}

	int MessageReader::Register(lua_State* L)
	{
		luaL_checkversion(L);

		luaL_Reg l[] =
		{
			{ "Read" , MessageReader::_Read },
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

	int MessageReader::_Read(lua_State* L)
	{
		ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
		char* data;
		int size;
		bool needFree = false;
		if (lua_isuserdata(L,2))
		{
			data = (char*)lua_touserdata(L,1);
			size = lua_tointeger(L,2);
			needFree = true;
		}
		else
			data = (char*)lua_tolstring(L,1,(size_t*)&size);

		void* ndata = malloc(size);
		int r = Zerounpack(data, size, ndata, &size);
		if (r < 0)
			return luaL_error(L, "Invalid unpack stream");
		if (r > size) 
		{
			free(ndata);
			ndata = malloc(r);
		}
		r = Zerounpack((void*)data, size, ndata, &r);

		lua_newtable(L);
		int index = 1;
		MessageReader* reader = new MessageReader((char*)ndata,r);
		while (reader->Left() > 0)
		{
			uint8 tt = reader->read<uint8>();
			if (tt == 0)
				break;	
		
			switch(tt)
			{
			case TYPE_BOOL:
				{
					lua_pushboolean(L,reader->read<bool>());
					lua_rawseti(L,-2,index++);
					break;
				}
			case TYPE_UINT8:
				{
					lua_pushinteger(L,reader->read<uint8>());
					lua_rawseti(L,-2,index++);
					break;
				}
			case TYPE_UINT16:
				{
					lua_pushinteger(L,reader->read<uint16>());
					lua_rawseti(L,-2,index++);
					break;
				}
			case TYPE_UINT32:
				{
					lua_pushinteger(L,reader->read<uint32>());
					lua_rawseti(L,-2,index++);
					break;
				}
			case TYPE_UINT64:
				{
					lua_pushnumber(L,reader->read<uint64>());
					lua_rawseti(L,-2,index++);
					break;
				}
			case TYPE_INT8:
				{
					lua_pushinteger(L,reader->read<int8>());
					lua_rawseti(L,-2,index++);
					break;
				}
			case TYPE_INT16:
				{
					lua_pushinteger(L,reader->read<int16>());
					lua_rawseti(L,-2,index++);
					break;
				}
			case TYPE_INT32:
				{
					lua_pushinteger(L,reader->read<int32>());
					lua_rawseti(L,-2,index++);
					break;
				}
			case TYPE_INT64:
				{
					lua_pushnumber(L,reader->read<int64>());
					lua_rawseti(L,-2,index++);
					break;
				}
			case TYPE_FLOAT:
				{
					lua_pushnumber(L,reader->read<float>());
					lua_rawseti(L,-2,index++);
					break;
				}
			case TYPE_DOUBLE:
				{
					lua_pushnumber(L,reader->read<double>());
					lua_rawseti(L,-2,index++);
					break;
				}
			case TYPE_STRING:
				{
					int len = reader->read<uint16>();
					lua_pushlstring(L,&reader->_data[reader->_pos],len);
					reader->_pos += len;
					lua_rawseti(L,-2,index++);
					break;
				}
			default:
				luaL_error(L,"error type:%d\n",tt);
			}
		}
		if (needFree)
			free((void*)data);
		
		return 1;
	}
}
