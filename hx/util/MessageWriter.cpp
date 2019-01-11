#include "MessageWriter.h"
#include "ZeroPack.h"
#include "ServerApp.h"

MessageWriter::MessageWriter(int size) : offset_(0), size_(size) {
	data_ = (char*)malloc(size);
}

MessageWriter::~MessageWriter(void) {
	free((void*)data_);
}

MessageWriter& MessageWriter::operator<<(bool value) {
	uint8_t type = TYPE_BOOL;
	Append(&type, (uint8_t*)&value, sizeof(bool));
	return *this;
}

MessageWriter& MessageWriter::operator<<(uint8_t value) {
	uint8_t type = TYPE_UINT8;
	Append(&type, (uint8_t*)&value, sizeof(uint8_t));
	return *this;
}

MessageWriter& MessageWriter::operator<<(uint16_t value) {
	uint8_t type = TYPE_UINT16;
	Append(&type, (uint8_t*)&value, sizeof(uint16_t));
	return *this;
}

MessageWriter& MessageWriter::operator<<(uint32_t value) {
	uint8_t type = TYPE_UINT32;
	Append(&type, (uint8_t*)&value, sizeof(uint32_t));
	return *this;
}

MessageWriter& MessageWriter::operator<<(uint64_t value) {
	uint8_t type = TYPE_UINT64;
	Append(&type, (uint8_t*)&value, sizeof(uint64_t));
	return *this;
}

MessageWriter& MessageWriter::operator<<(int8_t value) {
	uint8_t type = TYPE_INT8;
	Append(&type, (uint8_t*)&value, sizeof(float ));
	return *this;
}

MessageWriter& MessageWriter::operator<<(int16_t value) {
	uint8_t type = TYPE_INT16;
	Append(&type, (uint8_t*)&value, sizeof(int16_t));
	return *this;
}

MessageWriter& MessageWriter::operator<<(int32_t value) {
	uint8_t type = TYPE_INT32;
	Append(&type, (uint8_t*)&value, sizeof(int32_t));
	return *this;
}

MessageWriter& MessageWriter::operator<<(int64_t value) {
	uint8_t type = TYPE_INT64;
	Append(&type, (uint8_t*)&value, sizeof(int64_t));
	return *this;
}

MessageWriter& MessageWriter::operator<<(float value) {
	uint8_t type = TYPE_FLOAT;
	Append(&type, (uint8_t*)&value, sizeof(float));
	return *this;
}

MessageWriter& MessageWriter::operator<<(double value) {
	uint8_t type = TYPE_DOUBLE;
	Append(&type, (uint8_t*)&value, sizeof(double));
	return *this;
}

MessageWriter& MessageWriter::operator<<(const std::string& str) {
	Append((char*)str.c_str(), str.length());
	return *this;
}

MessageWriter& MessageWriter::operator<<(const char *str) {
	Append((char*)str, strlen(str));
	return *this;
}

char* MessageWriter::Data() {
	return data_;
}

int MessageWriter::Length() {
	return offset_;
}

void MessageWriter::Reserve(int cnt) {
	int size = size_;
	int need = offset_ + cnt;
	if (size < need) {
		while (size < need) {
			size = size * 2;
		}
		size_ = size;
		data_ = (char*)realloc(data_, size);
	}
}

void MessageWriter::Append(uint8_t* type, uint8_t* val, int cnt) {
	Reserve(cnt + 1);
	data_[offset_] = *type;
	offset_ += 1;
	memcpy((void*)&data_[offset_], val, cnt);
	offset_ += cnt;
}

void MessageWriter::Append(char* str, int size) {
	Reserve(size + 3);
	data_[offset_] = TYPE_STRING;
	offset_ += 1;
	memcpy(&data_[offset_], &size, 2);
	offset_ += 2;
	memcpy(&data_[offset_], str, size);
	offset_ += size;
}
