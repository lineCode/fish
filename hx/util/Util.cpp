#include "Util.h"
#include "Logger.h"
#include "rapidjson.h"
#include "filereadstream.h"
namespace Util {
	int LoadJson(rapidjson::Document& doc, const char* file) {
		FILE* F = fopen(file, "r");
		if (!F) {
			LOG_ERROR(fmt::format("LoadJson error:no such file:{}", file));
			return -1;
		}
		
		char readBuffer[1024];

		rapidjson::FileReadStream stream(F, readBuffer, sizeof(readBuffer));
		doc.ParseStream(stream);

		fclose(F);

		if (doc.HasParseError()) {
			LOG_ERROR(fmt::format("LoadJson error:{}", doc.GetParseError()));
			return -1;
		}

		return 0;
	}
};
