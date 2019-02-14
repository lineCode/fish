#include "LoggerServer.h"
#include "util/format.h"
#include "time/Timestamp.h"
#include <assert.h>

static const char kLOG_TAG[] = { 'D', 'I', 'W', 'E'};

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