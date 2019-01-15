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

	void WriteLog(const char* file, const char* source, int line, Loggerlevel level, uint64_t time, const char* content);

	void WriteLog(const char* file, void* data, size_t size);

	static void SetLoggerLevel(Loggerlevel level);
	
	static Loggerlevel GetLoggerLevel();

private:
	static Loggerlevel level_;

	std::mutex mutex_;

	LoggerInterface* interface_;
};

#define LOGGER 			Logger::GetSingleton()

#define LOG_DEBUG(x) 	LOGGER->WriteLog("runtime",__FILE__,__LINE__,Logger::eDebug,::Now(),(x).c_str())

#define LOG_INFO(x) 	LOGGER->WriteLog("runtime",__FILE__,__LINE__,Logger::eInfo,::Now(),(x).c_str())

#define LOG_WARN(x) 	LOGGER->WriteLog("runtime",__FILE__,__LINE__,Logger::eWarn,::Now(),(x).c_str())

#define LOG_ERROR(x) 	LOGGER->WriteLog("runtime",__FILE__,__LINE__,Logger::eError,::Now(),(x).c_str())


#endif
