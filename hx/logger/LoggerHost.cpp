#include "LoggerHost.h"
#include "util/format.h"
#include "time/Timestamp.h"
#include <assert.h>

static const char kLOG_TAG[] = { 'D', 'I', 'W', 'E'};

LoggerHost::LoggerHost(const char* path, bool show) : path_(path) {
	runtime_ = NULL;
	show_ = show;
}

LoggerHost::~LoggerHost(void) {
	std::map<std::string, FILE*>::iterator iter;
	for(iter = FILEMgr_.begin();iter != FILEMgr_.end();iter++) {
		FILE* F = iter->second;
		fflush(F);
		fclose(F);
	}
}

void LoggerHost::WriteLog(const char* file, const char* source, int line, int level, uint64_t time, const char* content) {
	FILE* F = NULL;
	std::map<std::string, FILE*>::iterator iter = FILEMgr_.find(file);
	if (iter == FILEMgr_.end()) {
		std::string path = fmt::format("{}/{}.log", path_, file);
		F = fopen(path.c_str(),"w");
		assert(F != NULL);
		FILEMgr_[file] = F;
	} else {
		F = iter->second;
	}

	struct tm stm;
	LocalTime((time_t)time, &stm);

	std::string date = fmt::format("{}-{}-{} {}:{}:{}", stm.tm_year+1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec);
	std::string log = fmt::format("[{}][{} @{}:{}] {}\r\n", kLOG_TAG[level], date, source, line, content);

	fwrite(log.c_str(), log.length() , 1, F);
	if (show_) {
		fwrite(log.c_str(), log.length() , 1, stderr);
	}
}

void LoggerHost::WriteLog(const char* file, void* data, size_t size) {
}


