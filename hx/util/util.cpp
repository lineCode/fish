#include "util.h"
#include "logger.h"

namespace Util {
	int LoadJson(rapidjson::Document& doc, const char* file) {
		FILE* F = fopen(file, "r");
		if (!F) {
			LOG_ERROR(fmt::format("LoadJson error:no such file:{}", file));
			return -1;
		}

		rapidjson::FileStream stream(F);
		doc.ParseStream(stream);

		fclose(F);

		if (doc.HasParseError()) {
			LOG_ERROR(fmt::format("LoadJson error:{}", doc.GetParseError()));
			return -1;
		}

		return 0;
	}
};