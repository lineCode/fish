#include "LoggerHost.h"
#include "util/format.h"
#include <assert.h>

LoggerHost::LoggerHost() {
	runtime_ = NULL;
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
	if (runtime_ == NULL) {
		std::map<std::string, FILE*>::iterator iter = FILEMgr_.find("runtime");
		if (iter == FILEMgr_.end()) {
			FILE* F = fopen("./runtime.log","w");
			assert(F != NULL);
			FILEMgr_["runtime"] = F;
			runtime_ = F;
		} else {
			runtime_ = iter->second;
		}
	}

	fwrite(log.c_str(), log.length() , 1, runtime_);
}

void LoggerHost::LuaLog(const char* file, std::string& log) {
	FILE* F = NULL;
	std::map<std::string, FILE*>::iterator iter = FILEMgr_.find(file);
	if (iter == FILEMgr_.end()) {
		std::string path = fmt::format("./{}.log", file);
		F = fopen(path.c_str(),"w");
		assert(F != NULL);
		FILEMgr_[file] = F;
	} else {
		F = iter->second;
	}
	fwrite(log.c_str(), log.length() , 1, F);
}


