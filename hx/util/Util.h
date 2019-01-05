#ifndef UTIL_H
#define UTIL_H
#include "document.h"  
#include <string>
#include "util/MemoryStream.h"

namespace Util {

	int LoadJson(rapidjson::Document& doc, const char* file);

	char* MakeMessage(char* data, size_t size, size_t* outSize);
	char* MakeMessage(std::string& data, size_t* outSize);
	char* MakeMessage(MemoryStream& data, size_t* outSize);
	
};

#endif