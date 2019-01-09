#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>
#include "document.h"
#include <string>
#include "util/MemoryStream.h"

namespace Util {
	void Exit(std::string reason);

	int LoadJson(rapidjson::Document& doc, const char* file);

	char* MakeMessage(char* data, size_t size, size_t* outSize);
	char* MakeMessage(std::string& data, size_t* outSize);
	char* MakeMessage(MemoryStream& data, size_t* outSize);
	
	uint16_t CheckSum(uint16_t* addr,size_t size);

	uint8_t* MessageEncrypt(uint16_t* wseed,uint16_t id,const uint8_t* message,size_t size);
	
	int MessageDecrypt(uint16_t* rseed,uint8_t* message,size_t size);
};

#endif