#ifndef LOGGER_H
#define LOGGER_H

#include <mutex>
#include <string>
#include "network/Address.h"
#include "util/Singleton.h"
#include "time/Timer.h"
#include "LoggerChannel.h"
#include "ServerApp.h"

class LoggerChannel;
class ServerApp;
class Logger : public Singleton<Logger> {
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
	Logger(Network::Addr& addr, ServerApp* app);
	~Logger(void);

	void Log(const char* file,int line,Loglevel level,const char* content);

	void LuaLog(const char* content);

	void OnChannelClose(void* userdata);

	void OnUpdate(Timer* timer, void* userdata);

	static void SetLogLevel(Loglevel level);
	
	static Loglevel LogLevel();
private:
	static Loglevel level_;
	std::mutex mutex_;
	Network::Addr addr_;
	LoggerChannel* channel_;
	Timer* timer_;
	ServerApp* app_;
	std::vector<std::string> cached_;
};

#define LOG_TRACE(x) Logger::GetSingleton()->Log(__FILE__,__LINE__,Logger::Trace,(x).c_str())

#define LOG_DEBUG(x) Logger::GetSingleton()->Log(__FILE__,__LINE__,Logger::Debug,(x).c_str())

#define LOG_INFO(x) Logger::GetSingleton()->Log(__FILE__,__LINE__,Logger::Info,(x).c_str())

#define LOG_WARN(x) Logger::GetSingleton()->Log(__FILE__,__LINE__,Logger::Warn,(x).c_str())

#define LOG_ERROR(x) Logger::GetSingleton()->Log(__FILE__,__LINE__,Logger::Error,(x).c_str())

#define LOG_FATAL(x) Logger::GetSingleton()->Log(__FILE__,__LINE__,Logger::Fatal,(x).c_str())

#endif
