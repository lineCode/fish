#include "Logger.h"
#include "util/format.h"
#include <iostream>

template <> 
Logger * Singleton<Logger>::singleton_ = 0;

Logger::Loglevel Logger::_level = Fatal;

Logger::Logger(void)
{
}


Logger::~Logger(void)
{
}

// void Logger::Log(const char* file,int line,Loglevel level,std::string& content)
// {
// 	Thread::MutexGuard guard(_metux);

// 	std::string log = fmt::format("@{}:{}: {}",file,line,content);
// 	std::cout << log << std::endl;
// }

void Logger::Log(const char* file,int line,Loglevel level,std::string content)
{
	Thread::MutexGuard guard(_metux);

	std::string log = fmt::format("@{}:{}: {}",file,line,content);
	std::cout << log << std::endl;
}

void Logger::LuaLog(std::string& content)
{
	Thread::MutexGuard guard(_metux);
	std::string log = fmt::format("@lua: {}",content);
	std::cout << log << std::endl;
}

void Logger::LuaLog(const char* content)
{
	Thread::MutexGuard guard(_metux);
	std::string log = fmt::format("@lua: {}",content);
	std::cout << log << std::endl;
}

void Logger::SetLogLevel(Loglevel level)
{
	Logger::_level = level;
}

Logger::Loglevel Logger::LogLevel()
{
	return Logger::_level;
}

Logger* Logger::CreateLogger()
{
	return new Logger();
}