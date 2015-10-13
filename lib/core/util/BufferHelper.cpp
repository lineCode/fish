#include "BufferHelper.h"


BufferHelper::BufferHelper(char* buffer,int size)
{
	_buffer = buffer;
	_pos = 0;
	_size = size;
}

BufferHelper::~BufferHelper()
{
}

BufferHelper& BufferHelper::operator>>(bool &value) 
{
	value = read<char>() > 0 ? true : false;
	return *this;
}

BufferHelper& BufferHelper::operator>>(uint8 &value) 
{
	value = read<uint8>();
	return *this;
}

BufferHelper& BufferHelper::operator>>(uint16 &value) 
{
	value = read<uint16>();
	return *this;
}

BufferHelper& BufferHelper::operator>>(uint32 &value) 
{
	value = read<uint32>();
	return *this;
}

BufferHelper& BufferHelper::operator>>(uint64 &value)
{
	value = read<uint64>();
	return *this;
}

BufferHelper& BufferHelper::operator>>(int8 &value) 
{
	value = read<int8>();
	return *this;
}

BufferHelper& BufferHelper::operator>>(int16 &value)
{
	value = read<int16>();
	return *this;
}

BufferHelper& BufferHelper::operator>>(int32 &value)
{
	value = read<int32>();
	return *this;
}

BufferHelper& BufferHelper::operator>>(int64 &value)
{
	value = read<int64>();
	return *this;
}

BufferHelper& BufferHelper::operator>>(float &value)
{
	value = read<float>();
	return *this;
}

BufferHelper& BufferHelper::operator>>(double &value) 
{
	value = read<double>();
	return *this;
}

BufferHelper& BufferHelper::operator>>(std::string& str)
{
	uint16 len = read<uint16>();
	if (_pos + len <= _size)
	{
		str.assign(_buffer+_pos,len);
		_pos += len;
	}
	else
	{
		str.assign(_buffer+_pos,_size-_pos);
		_pos = _size;
	}
	
	return *this;
}

char* BufferHelper::data()
{
	return (char*)_buffer;
}

char* BufferHelper::begin()
{ 
	if (_pos >= _size)
		return NULL;
	return (char*)_buffer+_pos; 
}

const char* BufferHelper::begin() const
{ 
	if (_pos >= _size)
		return NULL;
	return _buffer + _pos; 
}

int BufferHelper::pos()
{ 
	return _pos; 
}

void BufferHelper::pos(int p)
{
	if(p < 0)
		p = 0;
	_pos = p;
}

size_t BufferHelper::size() 
{
	return _size;
}

size_t BufferHelper::length()
{
	return _size - _pos;
}