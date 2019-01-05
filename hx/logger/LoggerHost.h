#ifndef LOGGER_HOST_H
#define LOGGER_HOST_H

#include <map>
#include "LoggerInterface.h"

class LoggerHost : public LoggerInterface {
public:
	LoggerHost(bool show = true);
	virtual ~LoggerHost();

	virtual void RuntimeLog(std::string& log);

	virtual void LuaLog(const char* file, std::string& log);

private:
	std::map<std::string, FILE*> FILEMgr_;
	FILE* runtime_;
	bool show_;
};
#endif
