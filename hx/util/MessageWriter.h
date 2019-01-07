#ifndef MESSAGEWRITER_H
#define MESSAGEWRITER_H
#include <stdint.h>
#include <string>
#include <vector>

namespace MessageHelper
{
	class MessageWriter
	{
	public:
		MessageWriter(int size = 64);
		~MessageWriter();

		MessageWriter& operator<<(bool value);
		MessageWriter& operator<<(uint8_t value);
		MessageWriter& operator<<(uint16_t value);
		MessageWriter& operator<<(uint32_t value);
		MessageWriter& operator<<(uint64_t value);
		MessageWriter& operator<<(int8_t value);
		MessageWriter& operator<<(int16_t value);
		MessageWriter& operator<<(int32_t value);
		MessageWriter& operator<<(int64_t value);
		MessageWriter& operator<<(float value);
		MessageWriter& operator<<(double value);
		MessageWriter& operator<<(const std::string& str);
		MessageWriter& operator<<(const char *str);

		MessageWriter& AppendString(const char* str,int size);
		
		char* Data();
		void Reset();
		int Length();

		void reserve(int cnt);
		void append(uint8_t* type, uint8_t* val, int size);
		void append(char* str,int size);

		char* _data;
		int _offset;
		int _size;
	};
}


#endif