#ifndef MESSAGEREADER_H
#define MESSAGEREADER_H
#include "Typedef.h"
#include <stdint.h>
#include <string>
#include <assert.h>


class MessageReader {
public:
	MessageReader(char* buffer,int size);
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

	template<typename T>
	T read() {
		T r = read<T>(pos_);
		pos_ += sizeof(T);
		return r;
	}

	template <typename T> 
	T read(size_t pos) {
		assert(sizeof(T) <= (size_t)(size_ - pos_));
		T val = *((T const*)&data_[pos]);
		return val;
	}

private:
	char* data_;
	int pos_;
	int size_;
};

#endif