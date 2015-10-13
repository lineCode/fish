#include "MemoryStream.h"
#include <stdint.h>

const char MemoryStream::kCRLF[] = "\r\n";

MemoryStream::MemoryStream(size_t size):_rpos(0),_wpos(0)
{
	_data.resize(size);
}

MemoryStream::MemoryStream(char* buffer,int size)
{
	_rpos = 0;
	_wpos = size;
	_data.assign(buffer,buffer+size);
}

MemoryStream::~MemoryStream()
{
}

MemoryStream& MemoryStream::operator<<(bool value) 
{
	append<bool>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint8 value) 
{
	append<uint8>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint16 value)
{
	append<uint16>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint32 value)
{
	append<uint32>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint64 value)
{
	append<uint64>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int8 value)
{
	append<int8>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int16 value)
{
	append<int16>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int32 value)
{
	append<int32>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int64 value) 
{
	append<int64>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(float value)
{
	append<float>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(double value)
{
	append<double>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(const std::string& value)
{
	append((const uint8 *)value.c_str(), value.length());
	append((uint8)0);
	return *this;
}

MemoryStream& MemoryStream::operator<<(const char *str) 
{
	append((uint8 const *)str, str ? strlen(str) : 0);
	append((uint8)0);
	return *this;
}

MemoryStream& MemoryStream::operator<<(MemoryStream& ms)
{
	append(ms.begin(),ms.length());
	append((uint8)0);
	return *this;
}

MemoryStream& MemoryStream::operator>>(bool &value) 
{
	value = read<char>() > 0 ? true : false;
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint8 &value) 
{
	value = read<uint8>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint16 &value) 
{
	value = read<uint16>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint32 &value) 
{
	value = read<uint32>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint64 &value)
{
	value = read<uint64>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int8 &value) 
{
	value = read<int8>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int16 &value)
{
	value = read<int16>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int32 &value)
{
	value = read<int32>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int64 &value)
{
	value = read<int64>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(float &value)
{
	value = read<float>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(double &value) 
{
	value = read<double>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(std::string& value)
{
	value.clear();
	while (length() > 0)
	{
		char c = read<char>();
		if (c == 0)
			break;
		value += c;
	}
	return *this;
}

MemoryStream& MemoryStream::operator>>(char *value) 
{
	while (length() > 0)
	{
		char c = read<char>();
		if (c == 0)
			break;
		*(value++) = c;
	}

	*value = '\0';
	return *this;
}

char* MemoryStream::data()
{
	return &_data[0];
}

char* MemoryStream::begin()
{ 
	if (_rpos >= _wpos)
		return NULL;
	return &*(_data.begin()+_rpos); 
}

const char* MemoryStream::begin() const
{ 
	return &*(_data.begin()+_rpos); 
}

char* MemoryStream::end()
{ 
	return &*(_data.begin()+_wpos); 
}

const char* MemoryStream::end() const
{ 
	return &*(_data.begin()+_wpos); 
}

int MemoryStream::rpos()
{ 
	return _rpos; 
}

void MemoryStream::rpos(int pos)
{
	if(pos < 0)
		pos = 0;
	_rpos = pos;
}

int MemoryStream::wpos() 
{ 
	return _wpos; 
}

void MemoryStream::wpos(int pos) 
{
	if(pos < 0)
		pos = 0;
	_wpos = pos;
}

size_t MemoryStream::length() 
{
	return rpos() >= wpos() ? 0 : wpos() - rpos();
}

size_t MemoryStream::size() 
{
	return _data.size();
}

void MemoryStream::resize(size_t newsize)
{
	_data.resize(newsize);
}

void MemoryStream::reserve(size_t ressize)
{
	if (ressize > size())
		_data.reserve(ressize);
}

void MemoryStream::clear() 
{
	_data.clear();
	_rpos = _wpos = 0;
}

void MemoryStream::reset()
{
	_rpos = _wpos = 0;
}

char* MemoryStream::peekData(int pos)
{
	assert((size_t)pos < _data.size());
	return &_data[pos];
}

void MemoryStream::retrieveUntil(const char* endc)
{
	assert(begin() <= endc);
	assert(endc <= end());
	_rpos += endc - begin();
}

const char* MemoryStream::findCRLF()
{
	const char* crlf = std::search(begin(),end(), kCRLF, kCRLF+2);
	return crlf == &_data[_wpos] ? NULL : crlf;
}

const char* MemoryStream::findCRLF(const char* start)
{
	assert(start <= end());
	const char* crlf = std::search((char*)start,end(), kCRLF, kCRLF+2);
	return crlf == &_data[_wpos] ? NULL : crlf;
}

const char* MemoryStream::findEOL()
{
	const void* eol = memchr(begin(), '\n', length());
	return static_cast<const char*>(eol);
}

const char* MemoryStream::findEOL(const char* start)
{
	const void* eol = memchr(start, '\n', length());
	return static_cast<const char*>(eol);
}

void MemoryStream::copyAll(char*& ptr,int & size) 
{
	size = length();
	ptr = (char*)malloc(size);
	memcpy(ptr,data()+ _rpos,size);
	_rpos += size;
}

void MemoryStream::copyWithSize(char* ptr,int size)
{
	assert(_wpos - _rpos >= size);
	memcpy(ptr,data()+ _rpos,size);
	_rpos += size;
}


int MemoryStream::Register(lua_State* L) 
{
	lua_newtable(L);

	lua_pushcfunction(L,MemoryStream::New);
	lua_setfield(L,-2,"New");
	lua_pushcfunction(L,MemoryStream::Delete);
	lua_setfield(L,-2,"Delete");

	lua_pushcfunction(L,MemoryStream::Write);
	lua_setfield(L,-2,"Write");

	lua_pushcfunction(L,MemoryStream::ReadBool);
	lua_setfield(L,-2,"ReadBool");

	lua_pushcfunction(L,MemoryStream::ReadInt);
	lua_setfield(L,-2,"ReadInt");

	lua_pushcfunction(L,MemoryStream::ReadNumber);
	lua_setfield(L,-2,"ReadNumber");

	lua_pushcfunction(L,MemoryStream::ReadString);
	lua_setfield(L,-2,"ReadString");

	lua_setglobal(L,"MemoryStreamCore");

	return 0;
}

int MemoryStream::New(lua_State* L) 
{
	MemoryStream* ms = new MemoryStream();
	lua_pushlightuserdata(L,ms);
	return 1;
}

int MemoryStream::Delete(lua_State* L) 
{
	MemoryStream* ms = (MemoryStream*)lua_touserdata(L,1);
	delete ms;
	return 0;
}

int MemoryStream::Write(lua_State* L)
{
	MemoryStream* ms = (MemoryStream*)lua_touserdata(L,1);
	int n = lua_gettop(L) - 1;
	if (n < 1)
		luaL_error(L,"Error MemoryStream::Write");

	for (int i = 1;i <= n;i++)
	{
		int index = i+1;
		int type = lua_type(L,index);
		switch(type)
		{
		case LUA_TNUMBER: 
			{
				int32_t x = (int32_t)lua_tointeger(L,index);
				lua_Number n = lua_tonumber(L,index);
				if ((lua_Number)x==n)
				{
					(*ms) << x;
				}
				else 
				{
					(*ms) << n;
				}
				break;
			}
		case LUA_TBOOLEAN: 
			{
				bool b = (bool)lua_toboolean(L,index);
				(*ms) << b;
				break;
			}
		case LUA_TSTRING: 
			{
				size_t sz = 0;
				const char *str = lua_tolstring(L,index,&sz);
				std::string value(str);
				(*ms) << value;
				break;
			}
		default:
			luaL_error(L, "Unsupport type %s to MemoryStream::Write", lua_typename(L, type));
		}
	}
	return 0;
}

int MemoryStream::ReadBool(lua_State* L) 
{
	MemoryStream* ms = (MemoryStream*)lua_touserdata(L,1);
	bool value;
	*ms >> value;
	lua_pushboolean(L,value);
	return 1;
}

int MemoryStream::ReadInt(lua_State* L) 
{
	MemoryStream* ms = (MemoryStream*)lua_touserdata(L,1);
	int value;
	*ms >> value;
	lua_pushinteger(L,value);
	return 1;
}

int MemoryStream::ReadNumber(lua_State* L) 
{
	MemoryStream* ms = (MemoryStream*)lua_touserdata(L,1);
	double value;
	*ms >> value;
	lua_pushnumber(L,value);
	return 1;
}

int MemoryStream::ReadString(lua_State* L) 
{
	MemoryStream* ms = (MemoryStream*)lua_touserdata(L,1);
	std::string value;
	*ms >> value;
	lua_pushlstring(L,value.c_str(),value.size());
	return 1;
}