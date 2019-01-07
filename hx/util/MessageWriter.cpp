#include "MessageWriter.h"
#include "ZeroPack.h"
#include "ServerApp.h"


namespace MessageHelper
{
	MessageWriter::MessageWriter(int size):_offset(0),_size(size)
	{
		_data = (char*)malloc(size);
	}

	MessageWriter::~MessageWriter(void)
	{
		free((void*)_data);
	}

	MessageWriter& MessageWriter::operator<<(bool value) 
	{
		uint8_t type = TYPE_BOOL;
		append(&type, (uint8_t*)&value, sizeof( bool ));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(uint8_t value) 
	{
		uint8_t type = TYPE_UINT8;
		append(&type, (uint8_t*)&value, sizeof( uint8_t ));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(uint16_t value)
	{
		uint8_t type = TYPE_UINT16;
		append(&type, (uint8_t*)&value, sizeof( uint16_t ));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(uint32_t value)
	{
		uint8_t type = TYPE_UINT32;
		append(&type, (uint8_t*)&value, sizeof( uint32_t ));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<( uint64_t value )
	{
		uint8_t type = TYPE_UINT64;
		append(&type, (uint8_t*)&value, sizeof( uint64_t ));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<( int8_t value )
	{
		uint8_t type = TYPE_INT8;
		append(&type, (uint8_t*)&value, sizeof( float ));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<( int16_t value )
	{
		uint8_t type = TYPE_INT16;
		append(&type, (uint8_t*)&value, sizeof( int16_t ));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<( int32_t value )
	{
		uint8_t type = TYPE_INT32;
		append(&type, (uint8_t*)&value, sizeof( int32_t ));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<( int64_t value )
	{
		uint8_t type = TYPE_INT64;
		append(&type, (uint8_t*)&value, sizeof( int64_t ));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(float value)
	{
		uint8_t type = TYPE_FLOAT;
		append(&type, (uint8_t*)&value, sizeof( float ));
		return *this;
	}

	MessageWriter& MessageWriter::operator<<(double value)
	{
		uint8_t type = TYPE_DOUBLE;
		append(&type, (uint8_t*)&value, sizeof( double ));
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

	void MessageWriter::append(uint8_t* type, uint8_t* val, int cnt)
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
}