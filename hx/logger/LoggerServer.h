#ifndef LOGGER_SERVER_H
#define LOGGER_SERVER_H

#include <unordered_map>
#include "LoggerInterface.h"

class LoggerServer : public LoggerInterface {
public:
	typedef std::unordered_map<std::string, FILE*> FILEMap;
public:
	LoggerServer(const char* path, bool show = true);
	virtual ~LoggerServer();

	virtual void WriteLog(const char* file, const char* source, int line, int level, double time, const char* content);

	virtual void WriteLog(const char* file, void* data, size_t size);

private:
	FILEMap fileCtx_;
	FILE* runtime_;
	std::string path_;
	bool show_;
};
#endif
