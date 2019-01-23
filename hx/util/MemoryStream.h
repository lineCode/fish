#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include <vector>
#include <string>
#include <algorithm>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

namespace EndianConvert {
	template<size_t T>
	inline void Convert(char* val) {
		std::swap(*val,*(val + T - 1));
		Convert<T-2>(val + 1);
	}

	template<>
	inline void Convert<0>(char* ) {
	}

	template<>
	inline void Convert<1>(char* ) {
	}

	template<typename T>
	inline void Apply(T* val) {
		Convert<sizeof(T)>((char*)val);
	}

	inline void Convert(char *val, size_t size) {
		if(size < 2)
			return;
		std::swap(*val, *(val + size - 1));
		Convert(val + 1, size - 2);
	}
};

class MemoryStream {
public:
	const static size_t		DEFAULT_SIZE = 0x100;
	static const char		kCRLF[];

	MemoryStream(size_t size = DEFAULT_SIZE);
	MemoryStream(char* buffer,int size);
	~MemoryStream();

	MemoryStream& operator<<(bool value) {
		Append<bool>(value);
		return *this;
	}

	MemoryStream& operator<<(uint8_t value) {
		Append<uint8_t>(value);
		return *this;
	}

	MemoryStream& operator<<(uint16_t value) {
		Append<uint16_t>(value);
		return *this;
	}

	MemoryStream& operator<<(uint32_t value) {
		Append<uint32_t>(value);
		return *this;
	}

	MemoryStream& operator<<(uint64_t value) {
		Append<uint64_t>(value);
		return *this;
	}

	MemoryStream& operator<<(int8_t value) {
		Append<int8_t>(value);
		return *this;
	}

	MemoryStream& operator<<(int16_t value) {
		Append<int16_t>(value);
		return *this;
	}

	MemoryStream& operator<<(int32_t value) {
		Append<int32_t>(value);
		return *this;
	}

	MemoryStream& operator<<(int64_t value) {
		Append<int64_t>(value);
		return *this;
	}

	MemoryStream& operator<<(float value) {
		Append<float>(value);
		return *this;
	}

	MemoryStream& operator<<(double value) {
		Append<double>(value);
		return *this;
	}

	MemoryStream& operator<<(const std::string& value) {
		Append((const uint8_t *)value.c_str(), value.length());
		Append((uint8_t)0);
		return *this;
	}

	MemoryStream& operator<<(const char *str) {
		Append((uint8_t const *)str, str ? strlen(str) : 0);
		Append((uint8_t)0);
		return *this;
	}

	MemoryStream& operator<<(MemoryStream& ms) {
		Append(ms.Begin(),ms.Length());
		Append((uint8_t)0);
		return *this;
	}

	MemoryStream& operator>>(bool &value) {
		value = Read<char>() > 0 ? true : false;
		return *this;
	}

	MemoryStream& operator>>(uint8_t &value) {
		value = Read<uint8_t>();
		return *this;
	}

	MemoryStream& operator>>(uint16_t &value) {
		value = Read<uint16_t>();
		return *this;
	}

	MemoryStream& operator>>(uint32_t &value) {
		value = Read<uint32_t>();
		return *this;
	}

	MemoryStream& operator>>(uint64_t &value) {
		value = Read<uint64_t>();
		return *this;
	}

	MemoryStream& operator>>(int8_t &value) {
		value = Read<int8_t>();
		return *this;
	}

	MemoryStream& operator>>(int16_t &value) {
		value = Read<int16_t>();
		return *this;
	}

	MemoryStream& operator>>(int32_t &value) {
		value = Read<int32_t>();
		return *this;
	}

	MemoryStream& operator>>(int64_t &value) {
		value = Read<int64_t>();
		return *this;
	}

	MemoryStream& operator>>(float &value) {
		value = Read<float>();
		return *this;
	}

	MemoryStream& operator>>(double &value) 
	{
		value = Read<double>();
		return *this;
	}

	MemoryStream& operator>>(std::string& value) {
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

	MemoryStream& operator>>(char *value) {
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

	char* Data() {
		return &data_[0];
	}

	char* Begin() { 
		if (readOffset_ >= writeOffset_)
			return NULL;
		return &data_[readOffset_]; 
	}

	const char* Begin() const { 
		if (readOffset_ >= writeOffset_)
			return NULL;
		return &data_[readOffset_]; 

	}

	char* End() { 
		return &data_[writeOffset_]; 
	}

	const char* End() const { 
		return &data_[writeOffset_]; 
	}

	int ReadOffset() { 
		return readOffset_; 
	}

	void ReadOffset(int pos) {
		if(pos < 0)
			pos = 0;
		readOffset_ = pos;
	}

	int WriteOffset() { 
		return writeOffset_; 
	}

	void WriteOffset(int pos) {
		if(pos < 0)
			pos = 0;
		writeOffset_ = pos;
	}

	size_t Length() {
		return ReadOffset() >= WriteOffset() ? 0 : WriteOffset() - ReadOffset();
	}

	size_t Size() {
		return data_.size();
	}

	void Resize(size_t newsize) {
		data_.resize(newsize);
	}

	void Reserve(size_t ressize) {
		if (ressize > Size())
			data_.reserve(ressize);
	}

	void Clear() {
		data_.clear();
		readOffset_ = writeOffset_ = 0;
	}

	void Reset() {
		readOffset_ = writeOffset_ = 0;
	}

	char* Peek(size_t size) {
		if ( size > Length() ) {
			return NULL;
		}
		return &data_[readOffset_];
	}

	void RetrieveUntil(const char* endc);

	const char* FindCRLF();
	const char* FindCRLF(const char* start);

	const char* FindEOL();
	const char* FindEOL(const char* start);

	void CopyAll(char*& ptr,int & size);
	void CopyWithSize(char* ptr,int size);

	template<typename T>
	void Append(T value) {
		Append((const uint8_t *)&value, sizeof( value ));
	}

	void Append(const char* c,size_t cnt) {
		Append((const uint8_t*)c, cnt);
	}

	void Append(const std::string& str) {
		Append(str.c_str(),str.size()+1);
	}

	void Append(const uint8_t* val, size_t cnt) {
		if (cnt == 0) {
			return;
		}
		if (data_.size() < writeOffset_ + cnt) {
			int size = data_.size();
			for (;;) {
				size = size * 2;
				if (size > writeOffset_ + (int)cnt) {
					data_.resize(size);
					break;
				}
			}
			
		}
		memcpy((void*)&data_[writeOffset_],val,cnt);
		writeOffset_ += cnt;
	}

	template<typename T>
	T Read()  {
		T r = Read<T>(readOffset_);
		readOffset_ += sizeof(T);
		return r;
	}

	template <typename T> 
	T Read(size_t pos) {
		assert(sizeof(T) <=  Length());
		T val = *((T const*)&data_[pos]);
		return val;
	}

	int readOffset_;
	int writeOffset_;
	std::vector<char> data_;
};

#endif