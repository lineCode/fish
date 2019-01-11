#ifndef STREAM_WRITER_H
#define STREAM_WRITER_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>


class StreamWriter {
public:
	StreamWriter(int size = 64);
	~StreamWriter();

	StreamWriter& operator<<(bool value);
	StreamWriter& operator<<(uint8_t value);
	StreamWriter& operator<<(uint16_t value);
	StreamWriter& operator<<(uint32_t value);
	StreamWriter& operator<<(uint64_t value);
	StreamWriter& operator<<(int8_t value);
	StreamWriter& operator<<(int16_t value);
	StreamWriter& operator<<(int32_t value);
	StreamWriter& operator<<(int64_t value);
	StreamWriter& operator<<(float value);
	StreamWriter& operator<<(double value);
	StreamWriter& operator<<(std::string& str);
	StreamWriter& operator<<(const char *str);

	template<typename T>
	void Append(T value) {
		Append((const uint8_t *)&value, sizeof( value ));
	}

	void Append(const char* c,size_t cnt) {
		Append((const uint8_t*)c, cnt);
	}

	void Append(const uint8_t* val, size_t cnt) {
		if (cnt == 0) {
			return;
		}
		if (data_.size() < offset_ + cnt) {
			int size = data_.size();
			for (;;) {
				size = size * 2;
				if (size > offset_ + (int)cnt) {
					data_.resize(size);
					break;
				}
			}
			
		}
		memcpy((void*)&data_[offset_],val,cnt);
		offset_ += cnt;
	}


	char* Data();
	int Length();

private:
	int offset_;
	std::vector<char> data_;
};


#endif
