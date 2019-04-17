﻿#include "LoggerServer.h"
#include "util/format.h"
#include "util/Util.h"
#include "time/Timestamp.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

static const char kLOG_TAG[] = { 'D', 'I', 'W', 'E'};

#ifdef WIN32
#define snprintf _snprintf
#endif

LoggerServer::LoggerServer(const char* path, bool show) : path_(path) {
	runtime_ = NULL;
	show_ = show;
}

LoggerServer::~LoggerServer(void) {
	FILEMap::iterator iter;
	for ( iter = fileCtx_.begin(); iter != fileCtx_.end(); iter++ ) {
		FILE* F = iter->second;
		fclose(F);
	}
}

void LoggerServer::Write(const char* file, const char* source, int line, int level, double time, const char* content) {
	FILE* F = GetFILE(file);

	struct tm stm;
	LocalTime((time_t)time, &stm);

	uint64_t decimal = (time - (time_t)time) * 1000;

	std::string date = fmt::format("{}-{:02}-{:02} {:02}:{:02}:{:02}", stm.tm_year+1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);
	if (decimal != 0) {
		date.append(fmt::format(".{:03}", decimal));
	}
	
	std::string log = fmt::format("[{}][{} @{}:{}] {}\r\n", kLOG_TAG[level], date, source, line, content);

	fwrite(log.c_str(), log.length() , 1, F);
	fflush(F);
	if (show_) {
		fwrite(log.c_str(), log.length() , 1, stderr);
	}
}

void LoggerServer::Write(const char* file, void* data, size_t size) {
}

FILE* LoggerServer::GetFILE(const char* file) {
	FILE* F = NULL;
	FILEMap::iterator iter = fileCtx_.find(file);
	if ( iter == fileCtx_.end() ) {
		std::string path = fmt::format("{}{}.log", path_, file);

		std::vector<std::string> result;
		Util::SplitString(path, "/", result);

		if ( result.empty() ) {
			F = fopen(path.c_str(), "w");
			assert(F != NULL);
		} else {
			for ( int i = 0; i < result.size() - 1; i++ ) {
				const std::string& path = result[i];
				_mkdir(path.c_str());
			}
			const std::string& file = result[result.size()];
			F = fopen(file.c_str(), "w");
			assert(F != NULL);
		}
	
		fileCtx_[file] = F;
	} else {
		F = iter->second;
	}
	return F;
}
