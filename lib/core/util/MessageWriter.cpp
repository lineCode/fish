#include "MessageWriter.h"
#include "ZeroPack.h"

#include "../ServerApp.h"


namespace MessageHelper
{
	MessageWriter::MessageWriter(int size):_offset(0),_size(size)
	{
		_data = (char*)malloc(size);
	}

	MessageWriter::~MessageWriter(void)
	{
	}

	MessageWriter& MessageWriter::operator<<(bool value) 
	{
		uint8 type = TYPE_BOOL;
		append(&type,(uint8*)&value,sizeof(bool));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(uint8 value) 
	{
		uint8 type = TYPE_UINT8;
		append(&type,(uint8*)&value,sizeof(uint8));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(uint16 value)
	{
		uint8 type = TYPE_UINT16;
		append(&type,(uint8*)&value,sizeof(uint16));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(uint32 value)
	{
		uint8 type = TYPE_UINT32;
		append(&type,(uint8*)&value,sizeof(uint32));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(uint64 value)
	{
		uint8 type = TYPE_UINT64;
		append(&type,(uint8*)&value,sizeof(uint64));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(int8 value)
	{
		uint8 type = TYPE_INT8;
		append(&type,(uint8*)&value,sizeof(float));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(int16 value)
	{
		uint8 type = TYPE_INT16;
		append(&type,(uint8*)&value,sizeof(int16));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(int32 value)
	{
		uint8 type = TYPE_INT32;
		append(&type,(uint8*)&value,sizeof(int32));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(int64 value) 
	{
		uint8 type = TYPE_INT64;
		append(&type,(uint8*)&value,sizeof(int64));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(float value)
	{
		uint8 type = TYPE_FLOAT;
		append(&type,(uint8*)&value,sizeof(float));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(double value)
	{
		uint8 type = TYPE_DOUBLE;
		append(&type,(uint8*)&value,sizeof(double));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(const std::string& str)
	{
		append((char*)str.c_str(),str.length());
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(const char *str) 
	{
		append((char*)str,strlen(str));
		return *this;
	}

	MessageWriter& MessageWriter::AppendString(const char* str,int size)
	{
		append((char*)str,size);
		return *this;
	}

	char* MessageWriter::Data()
	{
		return _data;
	}

	void MessageWriter::Reset()
	{
		_offset = 0;
	}

	int MessageWriter::Length()
	{
		return _offset;
	}

	void MessageWriter::reserve(int cnt)
	{
		int size = _size;
		int need = _offset + cnt;
		if (size < need)
		{
			while(size < need)
				size = size * 2;
			_size = size;
			_data = (char*)realloc(_data,size);
		}
	}

	void MessageWriter::append(uint8* type,uint8* val,int cnt)
	{
		reserve(cnt + 1);
		_data[_offset] = *type;
		_offset += 1;
		memcpy((void*)&_data[_offset],val,cnt);
		_offset += cnt;
	}

	void MessageWriter::append(char* str,int size)
	{
		reserve(size + 3);
		_data[_offset] = TYPE_STRING;
		_offset += 1;
		memcpy(&_data[_offset],&size,2);
		_offset += 2;
		memcpy(&_data[_offset],str,size);
		_offset += size;
	}

	int MessageWriter::Register(lua_State* L)
	{
		luaL_checkversion(L);

		luaL_Reg l[] =
		{
			{ "Write" , MessageWriter::_Write },
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

	int MessageWriter::_Write(lua_State* L)
	{
		ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

		MessageWriter* writer;
		app->WriterPool().Pop(writer);
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

		app->WriterPool().Push(writer);
		return 1;
	}

}