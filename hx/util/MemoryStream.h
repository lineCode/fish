#ifndef MEMORYSTREAM_H
#define MEMORYSTREAM_H

#include <vector>
#include <string>
#include <algorithm>

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "../Typedef.h"

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

	MemoryStream& operator<<(bool value);
	MemoryStream& operator<<(uint8_t value);
	MemoryStream& operator<<(uint16_t value);
	MemoryStream& operator<<(uint32_t value);
	MemoryStream& operator<<(uint64_t value);
	MemoryStream& operator<<(int8_t value);
	MemoryStream& operator<<(int16_t value);
	MemoryStream& operator<<(int32_t value);
	MemoryStream& operator<<(int64_t value);
	MemoryStream& operator<<(float value);
	MemoryStream& operator<<(double value);
	MemoryStream& operator<<(const std::string& value);
	MemoryStream& operator<<(const char *str);
	MemoryStream& operator<<(MemoryStream& ms);

	MemoryStream& operator>>(bool& value);
	MemoryStream& operator>>(uint8_t& value);
	MemoryStream& operator>>(uint16_t& value);
	MemoryStream& operator>>(uint32_t& value);
	MemoryStream& operator>>(uint64_t& value);
	MemoryStream& operator>>(int8_t& value);
	MemoryStream& operator>>(int16_t& value);
	MemoryStream& operator>>(int32_t& value);
	MemoryStream& operator>>(int64_t& value);
	MemoryStream& operator>>(float& value);
	MemoryStream& operator>>(double& value);
	MemoryStream& operator>>(std::string& value);
	MemoryStream& operator>>(char *value);

	char* Data();

	char* Begin();
	const char* Begin() const;

	char* End();
	const char* End() const;

	int ReadOffset();
	void ReadOffset(int offset);

	int WriteOffset();
	void WriteOffset(int offset);

	size_t Length();
	size_t Size();

	void Resize(size_t size);
	void Reserve(size_t size);

	void Clear();
	void Reset();

	char* Peek(int pos);
	void RetrieveUntil(const char* endc);

	const char* FindCRLF();
	const char* FindCRLF(const char* start);

	const char* FindEOL();
	const char* FindEOL(const char* start);

	void CopyAll(char*& ptr,int & size);
	void CopyWithSize(char* ptr,int size);
	
private:
	template<typename T>
	void Append(T value) {
		Append((const uint8 *)&value, sizeof(value));
	}

	void Append(const char* c,size_t cnt) {
		Append((const uint8*)c,cnt);
	}

	void Append(const std::string& str) {
		Append(str.c_str(),str.size()+1);
	}

	void Append(const uint8* val,size_t cnt) {
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