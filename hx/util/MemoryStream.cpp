#include "MemoryStream.h"
#include <stdint.h>

const char MemoryStream::kCRLF[] = "\r\n";

MemoryStream::MemoryStream(size_t size):readOffset_(0),writeOffset_(0) {
	data_.resize(size);
}

MemoryStream::MemoryStream(char* buffer,int size) {
	readOffset_ = 0;
	writeOffset_ = size;
	data_.assign(buffer, buffer + size);
}

MemoryStream::~MemoryStream() {
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
