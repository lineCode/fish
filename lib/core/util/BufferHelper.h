#ifndef BUFFERHELPER_H
#define BUFFERHELPER_H

#include <string>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../Typedef.h"


class BufferHelper
{
public:
	BufferHelper(char* buffer,int size);
	~BufferHelper();

	BufferHelper& operator>>(bool& value);
	BufferHelper& operator>>(uint8& value);
	BufferHelper& operator>>(uint16& value);
	BufferHelper& operator>>(uint32& value);
	BufferHelper& operator>>(uint64& value);
	BufferHelper& operator>>(int8& value);
	BufferHelper& operator>>(int16& value);
	BufferHelper& operator>>(int32& value);
	BufferHelper& operator>>(int64& value);
	BufferHelper& operator>>(float& value);
	BufferHelper& operator>>(double& value);
	BufferHelper& operator>>(std::string& value);

	char*		data();

	char*		begin();
	const char* begin() const;

	int			pos();
	void		pos(int p);

	size_t		size();
	size_t		length();

	template<typename T>
	T read() 
	{
		T r = read<T>(_pos);
		_pos += sizeof(T);
		return r;
	}

	template <typename T> 
	T read(size_t p)
	{
		assert(sizeof(T) + (size_t)_pos <=  (size_t)_size);
		T val = *((T const*)&_buffer[p]);
		return val;
	}

	char* read(int size)
	{
		assert(_pos + size <= _size);
		void* data = malloc(size);
		memcpy(data,(void*)(_buffer[_pos]),size);
		_pos += size;
		return (char*)data;
	}
protected:
	const char* _buffer;
	int _size;
	int _pos;
};

#endif