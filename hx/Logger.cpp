#include "Logger.h"
#include "util/format.h"
#include <iostream>

template <> 
Logger * Singleton<Logger>::singleton_ = 0;

Logger::Loglevel Logger::level_ = Fatal;

Logger::Logger(const char* file) {
	if (file != NULL) {
		FILE_ = fopen(file,"w");
		assert(FILE_ != NULL);
	} else {
		FILE_ = stdout;
	}
}

Logger::~Logger(void) {
	fflush(FILE_);
	fclose(FILE_);
}

void Logger::Log(const char* file,int line,Loglevel level,const char* content) {
	std::lock_guard<std::mutex> guard(mutex_);

	std::string log = fmt::format("@{}:{}: {}",file,line,content);
	fwrite(log.c_str(), log.length() , 1, FILE_);
	fprintf(FILE_,"\n");
}

void Logger::LuaLog(const char* content) {
	std::lock_guard<std::mutex> guard(mutex_);
	std::string log = fmt::format("@lua: {}",content);
	fwrite(log.c_str(), log.length() , 1, FILE_);
	fprintf(FILE_,"\n");
}

void Logger::SetLogLevel(Loglevel level) {
	Logger::level_ = level;
}

Logger::Loglevel Logger::LogLevel() {
	return Logger::level_;
}
