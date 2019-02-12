#ifndef LOGGER_SERVER_H
#define LOGGER_SERVER_H

#include <unordered_map>
#include "LoggerInterface.h"

class LoggerServer : public LoggerInterface {
public:
	LoggerServer(const char* path, bool show = true);
	virtual ~LoggerServer();

	virtual void WriteLog(const char* file, const char* source, int line, int level, uint64_t time, const char* content);

	virtual void WriteLog(const char* file, void* data, size_t size);

private:
	std::unordered_map<std::string, FILE*> fileCtx_;
	FILE* runtime_;
	std::string path_;
	bool show_;
};
#endif
