#ifndef UTIL_H
#define UTIL_H
#include "document.h"  

namespace Util {

	int LoadJson(rapidjson::Document& doc, const char* file);
};

#endif