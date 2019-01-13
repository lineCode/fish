#include "LoggerHost.h"
#include "util/format.h"
#include <assert.h>

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

void LoggerHost::RuntimeLog(std::string& log) {
	WriteLog("runtime", log);
}

void LoggerHost::WriteLog(const char* file, std::string& log) {
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
	fwrite(log.c_str(), log.length() , 1, F);
	if (show_) {
		fwrite(log.c_str(), log.length() , 1, stderr);
	}
}

void LoggerHost::WriteLog(const char* file, void* data, size_t size) {
}


