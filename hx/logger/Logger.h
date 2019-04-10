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
	enum Loggerlevel {
		eDebug = 0,
		eInfo,
		eWarn,
		eError,
	};
public:
	Logger(LoggerInterface* logger);

	~Logger(void);

	void WriteLog(const char* file, const char* source, int line, Loggerlevel level, double time, const char* content);

	void WriteLog(const char* file, const char* source, int line, Loggerlevel level, double time, std::string& content);

	void WriteLog(const char* file, void* data, size_t size);

	static void SetLoggerLevel(Loggerlevel level);
	
	static Loggerlevel GetLoggerLevel();

private:
	static Loggerlevel level_;

	std::mutex mutex_;

	LoggerInterface* interface_;
};

#define LOGGER 			Logger::GetSingleton()

#define LOG_DEBUG(x) 	LOGGER->WriteLog("runtime",__FILE__,__LINE__,Logger::eDebug,GetTimeMillis() / 1000,x)

#define LOG_INFO(x) 	LOGGER->WriteLog("runtime",__FILE__,__LINE__,Logger::eInfo,GetTimeMillis() / 1000,x)

#define LOG_WARN(x) 	LOGGER->WriteLog("runtime",__FILE__,__LINE__,Logger::eWarn,GetTimeMillis() / 1000,x)

#define LOG_ERROR(x) 	LOGGER->WriteLog("runtime",__FILE__,__LINE__,Logger::eError,GetTimeMillis() / 1000,x)


#endif
