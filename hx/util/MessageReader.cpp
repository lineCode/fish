#include "MessageReader.h"
#include <assert.h>
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
		uint8_t type = read<uint8_t>();
		assert(type == TYPE_BOOL);
		value = read<bool>();
		return *this;
	}

	MessageReader& MessageReader::operator>>( uint8_t &value )
	{
		uint8_t type = read<uint8_t>();
		assert(type == TYPE_UINT8);
		value = read<uint8_t>();
		return *this;
	}

	MessageReader& MessageReader::operator>>( uint16_t &value )
	{
		uint8_t type = read<uint8_t>();
		assert(type == TYPE_UINT16);
		value = read<uint16_t>();
		return *this;
	}

	MessageReader& MessageReader::operator>>( uint32_t &value )
	{
		uint8_t type = read<uint8_t>();
		assert(type == TYPE_UINT32);
		value = read<uint32_t>();
		return *this;
	}

	MessageReader& MessageReader::operator>>( uint64_t &value )
	{
		uint8_t type = read<uint8_t>();
		assert(type == TYPE_UINT64);
		value = read<bool>();
		return *this;
	}

	MessageReader& MessageReader::operator>>( int8_t &value )
	{
		uint8_t type = read<uint8_t>();
		assert(type == TYPE_INT8);
		value = read<int8_t>();
		return *this;
	}

	MessageReader& MessageReader::operator>>( int16_t &value )
	{
		uint8_t type = read<uint8_t>();
		assert(type == TYPE_UINT16);
		value = read<int16_t>();
		return *this;
	}

	MessageReader& MessageReader::operator>>( int32_t &value )
	{
		uint8_t type = read<uint8_t>();
		assert(type == TYPE_INT32);
		value = read<int32_t>();
		return *this;
	}

	MessageReader& MessageReader::operator>>( int64_t &value )
	{
		uint8_t type = read<uint8_t>();
		assert(type == TYPE_INT64);
		value = read<int64_t>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(float &value)
	{
		uint8_t type = read<uint8_t>();
		assert(type == TYPE_FLOAT);
		value = read<float>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(double &value) 
	{
		uint8_t type = read<uint8_t>();
		assert(type == TYPE_DOUBLE);
		value = read<double>();
		return *this;
	}

	MessageReader& MessageReader::operator>>(std::string& value)
	{
		value.clear();

		uint8_t type = read<uint8_t>();
		assert(type == TYPE_STRING);
		uint16_t len = read<uint16_t>();

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
