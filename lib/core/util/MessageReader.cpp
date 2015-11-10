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
}
