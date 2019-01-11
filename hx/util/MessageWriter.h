#ifndef MESSAGEWRITER_H
#define MESSAGEWRITER_H
#include <stdint.h>
#include <string>
#include <vector>


class MessageWriter {
public:
	MessageWriter(int size = 64);
	~MessageWriter();

	MessageWriter& operator<<(bool value);
	MessageWriter& operator<<(uint8_t value);
	MessageWriter& operator<<(uint16_t value);
	MessageWriter& operator<<(uint32_t value);
	MessageWriter& operator<<(uint64_t value);
	MessageWriter& operator<<(int8_t value);
	MessageWriter& operator<<(int16_t value);
	MessageWriter& operator<<(int32_t value);
	MessageWriter& operator<<(int64_t value);
	MessageWriter& operator<<(float value);
	MessageWriter& operator<<(double value);
	MessageWriter& operator<<(const std::string& str);
	MessageWriter& operator<<(const char *str);

	void Append(uint8_t* type, uint8_t* val, int size);
	void Append(char* str, int size);

	char* Data();
	int Length();

private:
	void Reserve(int cnt);

private:
	char* data_;
	int offset_;
	int size_;
};


#endif