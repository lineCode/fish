#ifndef LOGGER_INTERFACE_H
#define LOGGER_INTERFACE_H

#include <string>

class LoggerInterface {
public:
	LoggerInterface() {

	}

	virtual ~LoggerInterface() {

	}

	virtual void RuntimeLog(std::string& log) = 0;

	virtual void WriteLog(const char* file, std::string& log) = 0;

	virtual void WriteLog(const char* file, void* data, size_t size) = 0;
};

#endif
