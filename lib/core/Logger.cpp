#include "Logger.h"
#include "util/format.h"
#include <iostream>

template <> 
Logger * Singleton<Logger>::singleton_ = 0;

Logger::Loglevel Logger::_level = Fatal;

Logger::Logger(const char* file)
{
	if (file != NULL)
	{
		_handle = fopen(file,"w");
		assert(_handle != NULL);
	}
	else
		_handle = stdout;
}

Logger::~Logger(void)
{
	fflush(_handle);
	fclose(_handle);
}

void Logger::Log(const char* file,int line,Loglevel level,const char* content)
{
	Thread::MutexGuard guard(_metux);

	std::string log = fmt::format("@{}:{}: {}",file,line,content);
	fwrite(log.c_str(), log.length() , 1, _handle);
	fprintf(_handle,"\n");
}

void Logger::LuaLog(const char* content)
{
	Thread::MutexGuard guard(_metux);
	std::string log = fmt::format("@lua: {}",content);
	fwrite(log.c_str(), log.length() , 1, _handle);
	fprintf(_handle,"\n");
}

void Logger::SetLogLevel(Loglevel level)
{
	Logger::_level = level;
}

Logger::Loglevel Logger::LogLevel()
{
	return Logger::_level;
}

Logger* Logger::CreateLogger(const char* file)
{
	return new Logger(file);
}