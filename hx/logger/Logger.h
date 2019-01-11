#ifndef LOGGER_H
#define LOGGER_H

#include <mutex>
#include <string>
#include "util/Singleton.h"
#include "logger/LoggerInterface.h"
#include "ServerApp.h"

class LoggerChannel;
class ServerApp;

class Logger : public Singleton<Logger> {
public:
	enum Loglevel {
		eDebug,
		eInfo,
		eWarn,
		eError,
	};
public:
	Logger(LoggerInterface* logger);

	~Logger(void);

	void RuntimeLog(const char* file,int line,Loglevel level,const char* content);

	void WriteLog(const char* file, const char* content);

	void WriteLog(const char* file, void* data, size_t size);

	static void SetLogLevel(Loglevel level);
	
	static Loglevel LogLevel();

private:
	static Loglevel level_;

	std::mutex mutex_;

	LoggerInterface* interface_;
};

#define LOGGER 			Logger::GetSingleton()

#define LOG_DEBUG(x) 	LOGGER->RuntimeLog(__FILE__,__LINE__,Logger::eDebug,(x).c_str())

#define LOG_INFO(x) 	LOGGER->RuntimeLog(__FILE__,__LINE__,Logger::eInfo,(x).c_str())

#define LOG_WARN(x) 	LOGGER->RuntimeLog(__FILE__,__LINE__,Logger::eWarn,(x).c_str())

#define LOG_ERROR(x) 	LOGGER->RuntimeLog(__FILE__,__LINE__,Logger::eError,(x).c_str())


#endif
