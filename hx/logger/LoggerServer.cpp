#include "LoggerServer.h"
#include "util/format.h"
#include "time/Timestamp.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

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

void LoggerServer::WriteLog(const char* file, const char* source, int line, int level, double time, const char* content) {
	FILE* F = GetFILE(file);

	struct tm stm;
	LocalTime((time_t)time, &stm);

	uint64_t decimal = (time - (time_t)time) * 1000;

	// char date[64] = { 0 };
	// if (decimal != 0) {
	// 	snprintf(date, 64, "%d-%d-%d %d:%d:%d.%ld", stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, decimal);
	// } else {
	// 	snprintf(date, 64, "%d-%d-%d %d:%d:%d", stm.tm_year + 1900, stm.tm_mon + 1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);
	// }
	
	// char stack[128] = { 0 };
	// char* buff = stack;
	// int n = snprintf(buff, 128, "[%c][%s @%s:%d]", kLOG_TAG[level], date, source, line);

	// size_t len = strlen(content);
	// if ( n + len >= 126 ) {
	// 	buff = (char*)malloc(n + len + 2);
	// }
	// memcpy(buff + n, content, len);
	// memcpy(buff + n + len, "\r\n", 2);

	// fwrite(buff, n + len + 2, 1, F);
	// fflush(F);
	// // if ( show_ ) {
	// // 	fwrite(buff, n + len + 2, 1, stderr);
	// // }

	// if (buff != stack) {
	// 	free(buff);
	// }

	std::string date = fmt::format("{}-{}-{} {}:{}:{}", stm.tm_year+1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);
	if (decimal != 0) {
		date.append(fmt::format(".{}", decimal));
	}
	
	std::string log = fmt::format("[{}][{} @{}:{}] {}\r\n", kLOG_TAG[level], date, source, line, content);

	fwrite(log.c_str(), log.length() , 1, F);
	fflush(F);
	if (show_) {
		fwrite(log.c_str(), log.length() , 1, stderr);
	}
}

void LoggerServer::WriteLog(const char* file, void* data, size_t size) {
}

FILE* LoggerServer::GetFILE(const char* file) {
	FILE* F = NULL;
	FILEMap::iterator iter = fileCtx_.find(file);
	if ( iter == fileCtx_.end() ) {
		std::string path = fmt::format("{}{}.log", path_, file);
		F = fopen(path.c_str(),"w");
		assert(F != NULL);
		fileCtx_[file] = F;
	} else {
		F = iter->second;
	}
	return F;
}