#include "MessageWriter.h"
#include "ZeroPack.h"

#include "../ServerApp.h"


namespace MessageHelper
{
	MessageWriter::MessageWriter(int size):_pos(0)
	{
		_data.resize(size);
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
		appendStr((char*)str.c_str(),str.length());
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(const char *str) 
	{
		appendStr((char*)str,strlen(str));
		return *this;
	}

	MessageWriter& MessageWriter::AppendString(const char* str,int size)
	{
		appendStr((char*)str,size);
		return *this;
	}

	char* MessageWriter::Data()
	{
		return &_data[0];
	}

	int MessageWriter::Length()
	{
		return _pos;
	}

	void MessageWriter::Reset()
	{
		_pos = 0;
	}

	void MessageWriter::reserve(int cnt)
	{
		if (cnt == 0)
			return;
		if (_data.size() < _pos + cnt + 1)
		{
			int size = _data.size();
			for (;;)
			{
				size = size * 2;
				if (size > _pos + cnt)
				{
					_data.resize(size);
					break;
				}
			}

		}
	}
	void MessageWriter::append(uint8* type,uint8* val,int cnt)
	{
		reserve(cnt + 1);
		_data[_pos] = *type;
		_pos += 1;
		memcpy((void*)&_data[_pos],val,cnt);
		_pos += cnt;
	}

	void MessageWriter::appendStr(char* str,int size)
	{
		reserve(size + 3);

		_data[_pos] = TYPE_STRING;
		_pos += 1;
		memcpy(&_data[_pos],&size,2);
		_pos += 2;
		memcpy(&_data[_pos],str,size);
		_pos += size;
	}

	int MessageWriter::Register(lua_State* L)
	{
		luaL_checkversion(L);

		luaL_Reg l[] =
		{
			{ "Write" , MessageWriter::_Write },
			{ "New" , MessageWriter::_New },
			{ "Delete" , MessageWriter::_Delete },
			{ "Append" , MessageWriter::_Append },
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

	int MessageWriter::_New(lua_State* L)
	{
		ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));

		MessageWriter* writer;
		app->WriterPool().Pop(writer);
		writer->Reset();

		lua_pushlightuserdata(L,(void*)writer);

		return 1;
	}

	int MessageWriter::_Delete(lua_State* L)
	{
		ServerApp* app = (ServerApp*)lua_touserdata(L, lua_upvalueindex(1));
		MessageWriter* writer = (MessageWriter*)lua_touserdata(L,1);
		app->WriterPool().Push(writer);
		return 0;
	}

	int MessageWriter::_Append(lua_State* L)
	{
		return 0;
	}

}