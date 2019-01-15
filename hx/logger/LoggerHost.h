#ifndef LOGGER_HOST_H
#define LOGGER_HOST_H

#include <unordered_map>
#include "LoggerInterface.h"

class LoggerHost : public LoggerInterface {
public:
	LoggerHost(const char* path, bool show = true);
	virtual ~LoggerHost();

	virtual void WriteLog(const char* file, const char* source, int line, int level, uint64_t time, const char* content);

	virtual void WriteLog(const char* file, void* data, size_t size);

private:
	std::unordered_map<std::string, FILE*> fileCtx_;
	FILE* runtime_;
	std::string path_;
	bool show_;
};
#endif
