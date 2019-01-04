#include "MemoryStream.h"
#include <stdint.h>

const char MemoryStream::kCRLF[] = "\r\n";

MemoryStream::MemoryStream(size_t size):readOffset_(0),writeOffset_(0) {
	data_.resize(size);
}

MemoryStream::MemoryStream(char* buffer,int size) {
	readOffset_ = 0;
	writeOffset_ = size;
	data_.assign(buffer,buffer+size);
}

MemoryStream::~MemoryStream() {
}

MemoryStream& MemoryStream::operator<<(bool value) {
	Append<bool>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint8_t value) {
	Append<uint8_t>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint16_t value) {
	Append<uint16_t>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint32_t value) {
	Append<uint32_t>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint64_t value) {
	Append<uint64_t>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int8_t value) {
	Append<int8_t>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int16_t value) {
	Append<int16_t>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int32_t value) {
	Append<int32_t>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int64_t value) {
	Append<int64_t>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(float value) {
	Append<float>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(double value) {
	Append<double>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(const std::string& value) {
	Append((const uint8_t *)value.c_str(), value.length());
	Append((uint8_t)0);
	return *this;
}

MemoryStream& MemoryStream::operator<<(const char *str) {
	Append((uint8_t const *)str, str ? strlen(str) : 0);
	Append((uint8_t)0);
	return *this;
}

MemoryStream& MemoryStream::operator<<(MemoryStream& ms) {
	Append(ms.Begin(),ms.Length());
	Append((uint8_t)0);
	return *this;
}

MemoryStream& MemoryStream::operator>>(bool &value) {
	value = Read<char>() > 0 ? true : false;
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint8_t &value) {
	value = Read<uint8_t>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint16_t &value) {
	value = Read<uint16_t>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint32_t &value) {
	value = Read<uint32_t>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint64_t &value) {
	value = Read<uint64_t>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int8_t &value) {
	value = Read<int8_t>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int16_t &value) {
	value = Read<int16_t>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int32_t &value) {
	value = Read<int32_t>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int64_t &value) {
	value = Read<int64_t>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(float &value) {
	value = Read<float>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(double &value) 
{
	value = Read<double>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(std::string& value) {
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

MemoryStream& MemoryStream::operator>>(char *value) {
	while (Length() > 0) {
		char c = Read<char>();
		if (c == 0) {
			break;
		}
		*(value++) = c;
	}

	*value = '\0';
	return *this;
}

char* MemoryStream::Data() {
	return &data_[0];
}

char* MemoryStream::Begin() { 
	if (readOffset_ >= writeOffset_)
		return NULL;
	return &*(data_.begin()+readOffset_); 
}

const char* MemoryStream::Begin() const { 
	return &*(data_.begin()+readOffset_); 
}

char* MemoryStream::End() { 
	return &*(data_.begin()+writeOffset_); 
}

const char* MemoryStream::End() const { 
	return &*(data_.begin()+writeOffset_); 
}

int MemoryStream::ReadOffset() { 
	return readOffset_; 
}

void MemoryStream::ReadOffset(int pos) {
	if(pos < 0)
		pos = 0;
	readOffset_ = pos;
}

int MemoryStream::WriteOffset() { 
	return writeOffset_; 
}

void MemoryStream::WriteOffset(int pos) {
	if(pos < 0)
		pos = 0;
	writeOffset_ = pos;
}

size_t MemoryStream::Length() {
	return ReadOffset() >= WriteOffset() ? 0 : WriteOffset() - ReadOffset();
}

size_t MemoryStream::Size() {
	return data_.size();
}

void MemoryStream::Resize(size_t newsize) {
	data_.resize(newsize);
}

void MemoryStream::Reserve(size_t ressize) {
	if (ressize > Size())
		data_.reserve(ressize);
}

void MemoryStream::Clear() {
	data_.clear();
	readOffset_ = writeOffset_ = 0;
}

void MemoryStream::Reset() {
	readOffset_ = writeOffset_ = 0;
}

char* MemoryStream::Peek(int pos) {
	assert((size_t)pos < data_.size());
	return &data_[pos];
}

void MemoryStream::RetrieveUntil(const char* endc) {
	assert(Begin() <= endc);
	assert(endc <= End());
	readOffset_ += endc - Begin();
}

const char* MemoryStream::FindCRLF() {
	const char* crlf = std::search(Begin(),End(), kCRLF, kCRLF+2);
	return crlf == &data_[writeOffset_] ? NULL : crlf;
}

const char* MemoryStream::FindCRLF(const char* start) {
	assert(start <= End());
	const char* crlf = std::search((char*)start,End(), kCRLF, kCRLF+2);
	return crlf == &data_[writeOffset_] ? NULL : crlf;
}

const char* MemoryStream::FindEOL() {
	const void* eol = memchr(Begin(), '\n', Length());
	return static_cast<const char*>(eol);
}

const char* MemoryStream::FindEOL(const char* start) {
	const void* eol = memchr(start, '\n', Length());
	return static_cast<const char*>(eol);
}

void MemoryStream::CopyAll(char*& ptr,int & size) {
	size = Length();
	ptr = (char*)malloc(size);
	memcpy(ptr,Data()+ readOffset_,size);
	readOffset_ += size;
}

void MemoryStream::CopyWithSize(char* ptr,int size) {
	assert(writeOffset_ - readOffset_ >= size);
	memcpy(ptr,Data()+ readOffset_,size);
	readOffset_ += size;
}
