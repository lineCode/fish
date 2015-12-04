#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include "util/Singleton.h"
#include "thread/Mutex.h"

class Logger : public Singleton<Logger>
{
public:
	enum Loglevel
	{
		Trace,
		Debug,
		Info,
		Warn,
		Error,
		Fatal,
		LogEnd
	};
public:
	Logger(const char* file);
	~Logger(void);

	// void Log(const char* file,int line,Loglevel level,std::string& content);

	void Log(const char* file,int line,Loglevel level,const char* content);

	void LuaLog(const char* content);

	static void		SetLogLevel(Loglevel level);
	
	static Loglevel LogLevel();

	static Logger* CreateLogger(const char* file);

	static void ReleaseLogger();
private:
	Thread::Mutex _metux;

	static Loglevel _level;
	
	FILE* _handle;
};

#define LOG_TRACE(x) if (Logger::LogLevel() >= Logger::Trace) \
	Logger::GetSingleton().Log(__FILE__,__LINE__,Logger::Trace,(x).c_str())

#define LOG_DEBUG(x) if (Logger::LogLevel() >= Logger::Debug) \
	Logger::GetSingleton().Log(__FILE__,__LINE__,Logger::Debug,(x).c_str())

#define LOG_INFO(x) if (Logger::LogLevel() >= Logger::Info) \
	Logger::GetSingleton().Log(__FILE__,__LINE__,Logger::Info,(x).c_str())

#define LOG_WARN(x) if (Logger::LogLevel() >= Logger::Warn) \
	Logger::GetSingleton().Log(__FILE__,__LINE__,Logger::Warn,(x).c_str())

#define LOG_ERROR(x) if (Logger::LogLevel() >= Logger::Error) \
	Logger::GetSingleton().Log(__FILE__,__LINE__,Logger::Error,(x).c_str())

#define LOG_FATAL(x) if (Logger::LogLevel() >= Logger::Fatal) \
	Logger::GetSingleton().Log(__FILE__,__LINE__,Logger::Fatal,(x).c_str())

#endif