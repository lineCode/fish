#include "LuaMessageHelper.h"
#include "../util/MessageReader.h"
#include "../util/MessageWriter.h"

namespace LuaMessageHelper
{
	int _Read(lua_State* L)
	{
		MessageHelper::MessageReader* reader = (MessageHelper::MessageReader*)lua_touserdata(L, lua_upvalueindex(1));
		char* data;
		size_t size;
		bool needFree = false;
		if (lua_isuserdata(L,2))
		{
			data = (char*)lua_touserdata(L,1);
			size = lua_tointeger(L,2);
			needFree = true;
		}
		else
			data = (char*)lua_tolstring(L,1,(size_t*)&size);

		reader->SetBuffer(data,size);

		lua_newtable(L);
		int index = 1;

		while (reader->Left() > 0)
		{
			uint8 tt = reader->read<uint8>();
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

	int _Write(lua_State* L)
	{
		MessageHelper::MessageWriter* writer = (MessageHelper::MessageWriter*)lua_touserdata(L, lua_upvalueindex(2));
		writer->Reset();

		int len = lua_rawlen(L,-1);

		for (int i = 1;i <= len;i+=2)
		{
			lua_rawgeti(L,-1,i);
			int kt = lua_tointeger(L,-1);
			lua_pop(L,1);
			lua_rawgeti(L,-1,i+1);
			switch(kt)
			{
			case TYPE_BOOL:
				{
					(*writer) << (bool)lua_toboolean(L,-1);
					break;
				}
			case TYPE_UINT8:
				{
					(*writer) << (uint8)lua_tointeger(L,-1);
					break;
				}
			case TYPE_UINT16:
				{
					(*writer) << (uint16)lua_tointeger(L,-1);
					break;
				}
			case TYPE_UINT32:
				{
					(*writer) << (uint32)lua_tointeger(L,-1);
					break;
				}
			case TYPE_INT8:
				{
					(*writer) << (int8)lua_tointeger(L,-1);
					break;
				}
			case TYPE_INT16:
				{
					(*writer) << (int16)lua_tointeger(L,-1);
					break;
				}
			case TYPE_INT32:
				{
					(*writer) << (int32)lua_tointeger(L,-1);
					break;
				}
			case TYPE_INT64:
				{
					(*writer) << (int64)lua_tonumber(L,-1);
					break;
				}
			case TYPE_FLOAT:
				{
					(*writer) << (float)lua_tonumber(L,-1);
					break;
				}
			case TYPE_DOUBLE:
				{
					(*writer) << (double)lua_tonumber(L,-1);
					break;
				}
			case TYPE_STRING:
				{
					size_t size;
					const char* str = lua_tolstring(L,-1,&size);
					writer->AppendString(str,(int)size);
					break;
				}
			default:
				lua_pop(L,1);
				luaL_error(L,"error type:%d\n",kt);
			}
			lua_pop(L,1);
		}

		lua_pushlstring(L,writer->Data(),writer->Length());

		return 1;
	}

	int Register(lua_State* L)
	{
		luaL_checkversion(L);

		luaL_Reg l[] =
		{
			{ "Read", _Read },
			{ "Write", _Write },
			{ NULL, NULL },
		};

		luaL_newlibtable(L, l);

		MessageHelper::MessageReader* reader = new MessageHelper::MessageReader();
		MessageHelper::MessageWriter* writer = new MessageHelper::MessageWriter();

		lua_pushlightuserdata(L,reader);
		lua_pushlightuserdata(L,writer);

		luaL_setfuncs(L,l,2);
		return 1;
	}
}