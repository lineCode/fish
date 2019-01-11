#include "StreamReader.h"
#include <assert.h>
#include "ZeroPack.h"

StreamReader::StreamReader(char* buffer,int size):data_(buffer),offset_(0),size_(size) {
}

StreamReader::~StreamReader(void) {
}

StreamReader& StreamReader::operator>>(bool &value) {
	value = Read<char>() > 0 ? true : false;
	return *this;
}

StreamReader& StreamReader::operator>>(uint8_t &value) {
	value = Read<uint8_t>();
	return *this;
}

StreamReader& StreamReader::operator>>(uint16_t &value) {
	value = Read<uint16_t>();
	return *this;
}

StreamReader& StreamReader::operator>>(uint32_t &value) {
	value = Read<uint32_t>();
	return *this;
}

StreamReader& StreamReader::operator>>(uint64_t &value) {
	value = Read<uint64_t>();
	return *this;
}

StreamReader& StreamReader::operator>>(int8_t &value) {
	value = Read<int8_t>();
	return *this;
}

StreamReader& StreamReader::operator>>(int16_t &value) {
	value = Read<int16_t>();
	return *this;
}

StreamReader& StreamReader::operator>>(int32_t &value) {
	value = Read<int32_t>();
	return *this;
}

StreamReader& StreamReader::operator>>(int64_t &value) {
	value = Read<int64_t>();
	return *this;
}

StreamReader& StreamReader::operator>>(float &value) {
	value = Read<float>();
	return *this;
}

StreamReader& StreamReader::operator>>(double &value) {
	value = Read<double>();
	return *this;
}

StreamReader& StreamReader::operator>>(std::string& value) {
	value.clear();
	while (Length() > 0) {
		char c = Read<char>();
		if (c == 0) {
			break;
		}
		value += c;
	}
	return *this;
}

int StreamReader::Length() {
	return size_ - offset_;
}
