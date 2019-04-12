#include "Logger.h"
#include "network/Connector.h"
#include "util/format.h"

#include <iostream>
#include <assert.h>

template <> 
Logger * Singleton<Logger>::singleton_ = 0;

Logger::LogLv Logger::level_ = eDebug;

Logger::Logger(LoggerInterface* logger) {
	interface_ = logger;
}

Logger::~Logger(void) {
	delete interface_;
}

void Logger::Write(const char* file, const char* source, int line, LogLv level, double time, const char* content) {
	if (level < Logger::level_) {
		return;
	}
	std::lock_guard<std::mutex> guard(mutex_);
	interface_->Write(file, source, line, level, time, content);
}

void Logger::Write(const char* file, const char* source, int line, LogLv level, double time, std::string content) {
	Write(file, source, line, level, time, content.c_str());
}

void Logger::Write(const char* file, void* data, size_t size) {
	std::lock_guard<std::mutex> guard(mutex_);
	interface_->Write(file, data, size);
}

void Logger::SetLogLv(LogLv level) {
	Logger::level_ = level;
}

Logger::LogLv Logger::GetLogLv() {
	return Logger::level_;
}
