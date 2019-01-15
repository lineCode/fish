#include "Logger.h"
#include "network/Connector.h"
#include "util/format.h"

#include <iostream>
#include <assert.h>

template <> 
Logger * Singleton<Logger>::singleton_ = 0;

Logger::Loggerlevel Logger::level_ = eDebug;

Logger::Logger(LoggerInterface* logger) {
	interface_ = logger;
}

Logger::~Logger(void) {
	delete interface_;
}

void Logger::WriteLog(const char* file, const char* source, int line, Loggerlevel level, uint64_t time, const char* content) {
	if (level < Logger::level_) {
		return;
	}
	std::lock_guard<std::mutex> guard(mutex_);
	interface_->WriteLog(file, source, line, level, time, content);
}

void Logger::WriteLog(const char* file, void* data, size_t size) {
	std::lock_guard<std::mutex> guard(mutex_);
	interface_->WriteLog(file, data, size);
}

void Logger::SetLoggerLevel(Loggerlevel level) {
	Logger::level_ = level;
}

Logger::Loggerlevel Logger::GetLoggerLevel() {
	return Logger::level_;
}
