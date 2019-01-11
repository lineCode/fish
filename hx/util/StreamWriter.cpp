#include "StreamWriter.h"
#include "ZeroPack.h"
#include "ServerApp.h"

StreamWriter::StreamWriter(int size) : offset_(0) {
	data_.resize(size);
}

StreamWriter::~StreamWriter(void) {
}

StreamWriter& StreamWriter::operator<<(bool value) {
	Append<bool>(value);
	return *this;
}

StreamWriter& StreamWriter::operator<<(uint8_t value) {
	Append<uint8_t>(value);
	return *this;
}

StreamWriter& StreamWriter::operator<<(uint16_t value) {
	Append<uint16_t>(value);
	return *this;
}

StreamWriter& StreamWriter::operator<<(uint32_t value) {
	Append<uint32_t>(value);
	return *this;
}

StreamWriter& StreamWriter::operator<<(uint64_t value) {
	Append<uint64_t>(value);
	return *this;
}

StreamWriter& StreamWriter::operator<<(int8_t value) {
	Append<int8_t>(value);
	return *this;
}

StreamWriter& StreamWriter::operator<<(int16_t value) {
	Append<int16_t>(value);
	return *this;
}

StreamWriter& StreamWriter::operator<<(int32_t value) {
	Append<int32_t>(value);
	return *this;
}

StreamWriter& StreamWriter::operator<<(int64_t value) {
	Append<int64_t>(value);
	return *this;
}

StreamWriter& StreamWriter::operator<<(float value) {
	Append<float>(value);
	return *this;
}

StreamWriter& StreamWriter::operator<<(double value) {
	Append<double>(value);
	return *this;
}

StreamWriter& StreamWriter::operator<<(std::string& str) {
	Append((const uint8_t *)str.c_str(), str.length());
	Append((uint8_t)0);
	return *this;
}

StreamWriter& StreamWriter::operator<<(const char *str) {
	Append((uint8_t const *)str, str ? strlen(str) : 0);
	Append((uint8_t)0);
	return *this;
}

char* StreamWriter::Data() {
	return &data_[0];
}

int StreamWriter::Length() {
	return offset_;
}
