#ifndef STREAM_READER_H
#define STREAM_READER_H
#include "Typedef.h"
#include <stdint.h>
#include <string>
#include <assert.h>


class StreamReader {
public:
	StreamReader(char* buffer,int size);
	~StreamReader(void);

	StreamReader& operator>>(bool& value);
	StreamReader& operator>>(uint8_t& value);
	StreamReader& operator>>(uint16_t& value);
	StreamReader& operator>>(uint32_t& value);
	StreamReader& operator>>(uint64_t& value);
	StreamReader& operator>>(int8_t& value);
	StreamReader& operator>>(int16_t& value);
	StreamReader& operator>>(int32_t& value);
	StreamReader& operator>>(int64_t& value);
	StreamReader& operator>>(float& value);
	StreamReader& operator>>(double& value);
	StreamReader& operator>>(std::string& value);

	int Length();

	template<typename T>
	T Read()  {
		T r = Read<T>(offset_);
		offset_ += sizeof(T);
		return r;
	}

	template <typename T> 
	T Read(size_t pos) {
		assert(sizeof(T) <=  Length());
		T val = *((T const*)&data_[pos]);
		return val;
	}

	char* Read(size_t size) {
		if (Length() < (int)size) {
			return NULL;
		}
		int off = offset_;
		offset_ += size;
		return &data_[off];
	}

private:
	char* data_;
	int offset_;
	int size_;
};

#endif
