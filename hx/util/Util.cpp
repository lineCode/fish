#include "Util.h"
#include "rapidjson.h"
#include "filereadstream.h"
namespace Util {
	int LoadJson(rapidjson::Document& doc, const char* file) {
		FILE* F = fopen(file, "r");
		if (!F) {
			return -1;
		}
		
		char readBuffer[1024];

		rapidjson::FileReadStream stream(F, readBuffer, sizeof(readBuffer));
		doc.ParseStream(stream);

		fclose(F);

		if (doc.HasParseError()) {
			return -1;
		}

		return 0;
	}

	char* MakeMessage(char* data, size_t size, size_t* outSize) {
		ushort length = size + 2;
		char* content = (char*)malloc(length);
		memcpy(content, &length, 2);
		memcpy(content+2, data, size);
		*outSize = length;
		return content;
	}

	char* MakeMessage(std::string& data, size_t* outSize) {
		return MakeMessage((char*)data.c_str(), data.length(), outSize);
	}

	char* MakeMessage(MemoryStream& data, size_t* outSize) {
		return MakeMessage(data.Data(), data.Length(), outSize);
	}
};
