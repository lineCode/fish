#include "Logger.h"
#include "network/Connector.h"
#include "util/format.h"
#include <iostream>
#include <assert.h>

template <> 
Logger * Singleton<Logger>::singleton_ = 0;

Logger::Loglevel Logger::level_ = eDebug;

Logger::Logger(LoggerInterface* logger) {
	interface_ = logger;
}

Logger::~Logger(void) {
	delete interface_;
}

void Logger::RuntimeLog(const char* file,int line,Loglevel level,const char* content) {
	if (level < Logger::level_) {
		return;
	}
	std::lock_guard<std::mutex> guard(mutex_);
	std::string log = fmt::format("@{}:{}: {}\n", file, line, content);

	interface_->RuntimeLog(log);
}

void Logger::WriteLog(const char* file, const char* content) {
	std::lock_guard<std::mutex> guard(mutex_);
	std::string log(content);
	interface_->WriteLog(file, log);
}

void Logger::WriteLog(const char* file, void* data, size_t size) {
	std::lock_guard<std::mutex> guard(mutex_);
	interface_->WriteLog(file, data, size);
}

void Logger::SetLogLevel(Loglevel level) {
	Logger::level_ = level;
}

Logger::Loglevel Logger::LogLevel() {
	return Logger::level_;
}
