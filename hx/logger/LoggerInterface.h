#ifndef LOGGER_INTERFACE_H
#define LOGGER_INTERFACE_H
#include <stdint.h>
#include <string>

class LoggerInterface {
public:
	LoggerInterface() {}

	virtual ~LoggerInterface() {}

	virtual void Write(const char* file, const char* source, int line, int level, double time, const char* content) = 0;

	virtual void Write(const char* file, void* data, size_t size) = 0;
};

#endif
