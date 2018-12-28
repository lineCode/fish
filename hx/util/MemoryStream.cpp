#include "MemoryStream.h"
#include <stdint.h>

const char MemoryStream::kCRLF[] = "\r\n";

MemoryStream::MemoryStream(size_t size):rpos_(0),wpos_(0) {
	data_.resize(size);
}

MemoryStream::MemoryStream(char* buffer,int size) {
	rpos_ = 0;
	wpos_ = size;
	data_.assign(buffer,buffer+size);
}

MemoryStream::~MemoryStream() {
}

MemoryStream& MemoryStream::operator<<(bool value) {
	append<bool>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint8 value) {
	append<uint8>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint16 value) {
	append<uint16>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint32 value) {
	append<uint32>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(uint64 value) {
	append<uint64>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int8 value) {
	append<int8>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int16 value) {
	append<int16>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int32 value) {
	append<int32>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(int64 value) {
	append<int64>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(float value) {
	append<float>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(double value) {
	append<double>(value);
	return *this;
}

MemoryStream& MemoryStream::operator<<(const std::string& value) {
	append((const uint8 *)value.c_str(), value.length());
	append((uint8)0);
	return *this;
}

MemoryStream& MemoryStream::operator<<(const char *str) {
	append((uint8 const *)str, str ? strlen(str) : 0);
	append((uint8)0);
	return *this;
}

MemoryStream& MemoryStream::operator<<(MemoryStream& ms) {
	append(ms.begin(),ms.length());
	append((uint8)0);
	return *this;
}

MemoryStream& MemoryStream::operator>>(bool &value) {
	value = read<char>() > 0 ? true : false;
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint8 &value) {
	value = read<uint8>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint16 &value) {
	value = read<uint16>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint32 &value) {
	value = read<uint32>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(uint64 &value) {
	value = read<uint64>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int8 &value) {
	value = read<int8>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int16 &value) {
	value = read<int16>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int32 &value) {
	value = read<int32>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(int64 &value) {
	value = read<int64>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(float &value) {
	value = read<float>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(double &value) 
{
	value = read<double>();
	return *this;
}

MemoryStream& MemoryStream::operator>>(std::string& value) {
	value.clear();
	while (length() > 0) {
		char c = read<char>();
		if (c == 0) {
			break;
		}
		value += c;
	}
	return *this;
}

MemoryStream& MemoryStream::operator>>(char *value) {
	while (length() > 0) {
		char c = read<char>();
		if (c == 0) {
			break;
		}
		*(value++) = c;
	}

	*value = '\0';
	return *this;
}

char* MemoryStream::data() {
	return &data_[0];
}

char* MemoryStream::begin() { 
	if (rpos_ >= wpos_)
		return NULL;
	return &*(data_.begin()+rpos_); 
}

const char* MemoryStream::begin() const { 
	return &*(data_.begin()+rpos_); 
}

char* MemoryStream::end() { 
	return &*(data_.begin()+wpos_); 
}

const char* MemoryStream::end() const { 
	return &*(data_.begin()+wpos_); 
}

int MemoryStream::rpos() { 
	return rpos_; 
}

void MemoryStream::rpos(int pos) {
	if(pos < 0)
		pos = 0;
	rpos_ = pos;
}

int MemoryStream::wpos() { 
	return wpos_; 
}

void MemoryStream::wpos(int pos) {
	if(pos < 0)
		pos = 0;
	wpos_ = pos;
}

size_t MemoryStream::length() {
	return rpos() >= wpos() ? 0 : wpos() - rpos();
}

size_t MemoryStream::size() {
	return data_.size();
}

void MemoryStream::resize(size_t newsize) {
	data_.resize(newsize);
}

void MemoryStream::reserve(size_t ressize) {
	if (ressize > size())
		data_.reserve(ressize);
}

void MemoryStream::clear() {
	data_.clear();
	rpos_ = wpos_ = 0;
}

void MemoryStream::reset() {
	rpos_ = wpos_ = 0;
}

char* MemoryStream::peekData(int pos) {
	assert((size_t)pos < data_.size());
	return &data_[pos];
}

void MemoryStream::retrieveUntil(const char* endc) {
	assert(begin() <= endc);
	assert(endc <= end());
	rpos_ += endc - begin();
}

const char* MemoryStream::findCRLF() {
	const char* crlf = std::search(begin(),end(), kCRLF, kCRLF+2);
	return crlf == &data_[wpos_] ? NULL : crlf;
}

const char* MemoryStream::findCRLF(const char* start) {
	assert(start <= end());
	const char* crlf = std::search((char*)start,end(), kCRLF, kCRLF+2);
	return crlf == &data_[wpos_] ? NULL : crlf;
}

const char* MemoryStream::findEOL() {
	const void* eol = memchr(begin(), '\n', length());
	return static_cast<const char*>(eol);
}

const char* MemoryStream::findEOL(const char* start) {
	const void* eol = memchr(start, '\n', length());
	return static_cast<const char*>(eol);
}

void MemoryStream::copyAll(char*& ptr,int & size) {
	size = length();
	ptr = (char*)malloc(size);
	memcpy(ptr,data()+ rpos_,size);
	rpos_ += size;
}

void MemoryStream::copyWithSize(char* ptr,int size) {
	assert(wpos_ - rpos_ >= size);
	memcpy(ptr,data()+ rpos_,size);
	rpos_ += size;
}
