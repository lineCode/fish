#include "MessageReader.h"
#include <assert.h>
#include "ZeroPack.h"

MessageReader::MessageReader(char* buffer,int size):data_(buffer),pos_(0),size_(size) {
}

MessageReader::~MessageReader(void) {
}

MessageReader& MessageReader::operator>>(bool &value) {
	uint8_t type = read<uint8_t>();
	value = read<bool>();
	return *this;
}

MessageReader& MessageReader::operator>>(uint8_t &value) {
	uint8_t type = read<uint8_t>();
	value = read<uint8_t>();
	return *this;
}

MessageReader& MessageReader::operator>>(uint16_t &value) {
	uint8_t type = read<uint8_t>();
	value = read<uint16_t>();
	return *this;
}

MessageReader& MessageReader::operator>>(uint32_t &value) {
	uint8_t type = read<uint8_t>();
	value = read<uint32_t>();
	return *this;
}

MessageReader& MessageReader::operator>>(uint64_t &value) {
	uint8_t type = read<uint8_t>();
	value = read<bool>();
	return *this;
}

MessageReader& MessageReader::operator>>(int8_t &value) {
	uint8_t type = read<uint8_t>();
	value = read<int8_t>();
	return *this;
}

MessageReader& MessageReader::operator>>(int16_t &value) {
	uint8_t type = read<uint8_t>();
	value = read<int16_t>();
	return *this;
}

MessageReader& MessageReader::operator>>(int32_t &value) {
	uint8_t type = read<uint8_t>();
	value = read<int32_t>();
	return *this;
}

MessageReader& MessageReader::operator>>(int64_t &value) {
	uint8_t type = read<uint8_t>();
	value = read<int64_t>();
	return *this;
}

MessageReader& MessageReader::operator>>(float &value) {
	uint8_t type = read<uint8_t>();
	value = read<float>();
	return *this;
}

MessageReader& MessageReader::operator>>(double &value) {
	uint8_t type = read<uint8_t>();
	value = read<double>();
	return *this;
}

MessageReader& MessageReader::operator>>(std::string& value) {
	value.clear();

	uint8_t type = read<uint8_t>();
	uint16_t len = read<uint16_t>();

	value.assign(&data_[pos_],len);
	pos_ += len;
	return *this;
}
