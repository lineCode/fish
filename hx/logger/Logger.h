#ifndef LOGGER_H
#define LOGGER_H

#include <mutex>
#include <string>
#include "util/Singleton.h"
#include "time/Timestamp.h"
#include "logger/LoggerInterface.h"
#include "ServerApp.h"

class LoggerChannel;
class ServerApp;

class Logger : public Singleton<Logger> {
public:
	enum LogLv {
		eDebug = 0,
		eInfo,
		eWarn,
		eError,
	};
public:
	Logger(LoggerInterface* logger);

	~Logger(void);

	void Write(const char* file, const char* source, int line, LogLv level, double time, const char* content);

	void Write(const char* file, const char* source, int line, LogLv level, double time, std::string content);

	void Write(const char* file, void* data, size_t size);

	static void SetLogLv(LogLv level);
	
	static LogLv GetLogLv();

private:
	static LogLv level_;

	std::mutex mutex_;

	LoggerInterface* interface_;
};

#define LOGGER 			Logger::GetSingleton()

#define LOG_DEBUG(x) 	LOGGER->Write("runtime",__FILE__,__LINE__,Logger::eDebug,	GetTimeMillis() / 1000,x)

#define LOG_INFO(x) 	LOGGER->Write("runtime",__FILE__,__LINE__,Logger::eInfo,		GetTimeMillis() / 1000,x)

#define LOG_WARN(x) 	LOGGER->Write("runtime",__FILE__,__LINE__,Logger::eWarn,		GetTimeMillis() / 1000,x)

#define LOG_ERROR(x) 	LOGGER->Write("runtime",__FILE__,__LINE__,Logger::eError,	GetTimeMillis() / 1000,x)


#endif
