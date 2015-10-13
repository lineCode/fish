#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include <vector>
#include <string>
#include <algorithm>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../Typedef.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

namespace EndianConvert 
{
	template<size_t T>
	inline void Convert(char* val)
	{
		std::swap(*val,*(val + T - 1));
		Convert<T-2>(val + 1);
	}

	template<>
	inline void Convert<0>(char* )
	{
	}

	template<>
	inline void Convert<1>(char* )
	{
	}

	template<typename T>
	inline void Apply(T* val) 
	{
		Convert<sizeof(T)>((char*)val);
	}

	inline void Convert(char *val, size_t size) 
	{
		if(size < 2)
			return;
		std::swap(*val, *(val + size - 1));
		Convert(val + 1, size - 2);
	}
};

class MemoryStream
{
public:
	const static size_t		DEFAULT_SIZE = 0x100;
	static const char		kCRLF[];


	MemoryStream(size_t size = DEFAULT_SIZE);
	MemoryStream(char* buffer,int size);
	~MemoryStream();

	MemoryStream& operator<<(bool value);
	MemoryStream& operator<<(uint8 value);
	MemoryStream& operator<<(uint16 value);
	MemoryStream& operator<<(uint32 value);
	MemoryStream& operator<<(uint64 value);
	MemoryStream& operator<<(int8 value);
	MemoryStream& operator<<(int16 value);
	MemoryStream& operator<<(int32 value);
	MemoryStream& operator<<(int64 value);
	MemoryStream& operator<<(float value);
	MemoryStream& operator<<(double value);
	MemoryStream& operator<<(const std::string& value);
	MemoryStream& operator<<(const char *str);
	MemoryStream& operator<<(MemoryStream& ms);

	MemoryStream& operator>>(bool& value);
	MemoryStream& operator>>(uint8& value);
	MemoryStream& operator>>(uint16& value);
	MemoryStream& operator>>(uint32& value);
	MemoryStream& operator>>(uint64& value);
	MemoryStream& operator>>(int8& value);
	MemoryStream& operator>>(int16& value);
	MemoryStream& operator>>(int32& value);
	MemoryStream& operator>>(int64& value);
	MemoryStream& operator>>(float& value);
	MemoryStream& operator>>(double& value);
	MemoryStream& operator>>(std::string& value);
	MemoryStream& operator>>(char *value);

	char*		data();

	char*		begin();
	const char* begin() const;

	char*		end();
	const char* end() const;

	int			rpos();
	void		rpos(int rpos);

	int			wpos();
	void		wpos(int wpos);

	size_t		length();
	size_t		size();

	void		resize(size_t size);
	void		reserve(size_t size);

	void		clear();
	void		reset();

	char*		peekData(int pos);
	void		retrieveUntil(const char* endc);


	const char* findCRLF();
	const char* findCRLF(const char* start);

	const char* findEOL();
	const char* findEOL(const char* start);

	void		copyAll(char*& ptr,int & size);
	void		copyWithSize(char* ptr,int size);
	
	
	//for lua
	static int Register(lua_State* L);
	static int New(lua_State* L);
	static int Delete(lua_State* L);
	static int Write(lua_State* L);
	static int ReadBool(lua_State* L);
	static int ReadInt(lua_State* L);
	static int ReadNumber(lua_State* L);
	static int ReadString(lua_State* L);

public:
	template<typename T>
	void append(T value)
	{
		append((const uint8 *)&value, sizeof(value));
	}

	void append(const char* c,size_t cnt) 
	{
		append((const uint8*)c,cnt);
	}

	void append(const std::string& str)
	{
		append(str.c_str(),str.size()+1);
	}

	void append(const uint8* val,size_t cnt) 
	{
		if (cnt == 0)
			return;
		if (_data.size() < _wpos + cnt)
		{
			int size = _data.size();
			for (;;)
			{
				size = size * 2;
				if (size > _wpos + (int)cnt)
				{
					_data.resize(size);
					break;
				}
			}
			
		}
		memcpy((void*)&_data[_wpos],val,cnt);
		_wpos += cnt;
	}

	template<typename T>
	T read() 
	{
		T r = read<T>(_rpos);
		_rpos += sizeof(T);
		return r;
	}

	template <typename T> 
	T read(size_t pos)
	{
		assert(sizeof(T) <=  length());
		T val = *((T const*)&_data[pos]);
		return val;
	}
protected:
	
	int					_rpos;
	int					_wpos;
	std::vector<char>   _data;
};

#endif