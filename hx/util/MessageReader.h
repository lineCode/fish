#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H
#include "Typedef.h"
#include <stdint.h>
#include <string>
#include <assert.h>

namespace MessageHelper
{
	class MessageReader
	{
	public:
		MessageReader(char* buffer,int size);
		MessageReader();
		~MessageReader(void);

		MessageReader& operator>>(bool& value);
		MessageReader& operator>>(uint8_t& value);
		MessageReader& operator>>(uint16_t& value);
		MessageReader& operator>>(uint32_t& value);
		MessageReader& operator>>(uint64_t& value);
		MessageReader& operator>>(int8_t& value);
		MessageReader& operator>>(int16_t& value);
		MessageReader& operator>>(int32_t& value);
		MessageReader& operator>>(int64_t& value);
		MessageReader& operator>>(float& value);
		MessageReader& operator>>(double& value);
		MessageReader& operator>>(std::string& value);

		void SetBuffer(char* buffer,int size);

		int Pos();

		int Left();

		int Length();

		template<typename T>
		T read() 
		{
			T r = read<T>(_pos);
			_pos += sizeof(T);
			return r;
		}

		template <typename T> 
		T read(size_t pos)
		{
			assert(sizeof(T) <= (size_t)Left());
			T val = *((T const*)&_data[pos]);
			return val;
		}

		char* _data;
		int _pos;
		int _size;
	};

}

#endif