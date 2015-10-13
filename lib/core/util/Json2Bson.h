#pragma once

#include "rapidjson/reader.h"
#include "rapidjson/rapidjson.h"
#include "bson/src/bson.h"
#include <stack>
#include <iostream>
#include <string>

using namespace rapidjson;

struct Json2Bson
{
	std::stack<bson::BSONObjBuilder*> stackBuilder_;
	bson::BSONObjBuilder* builder_;
	std::stack<std::string> stackField_;
	std::string field_;
	bool isArray_;
	bson::BSONObj bsonObj_;
	

	Json2Bson();

	bool Null();

	bool Bool(bool b);

	bool Int(int i);

	bool Uint(unsigned u);

	bool Int64(int64_t i);

	bool Uint64(uint64_t u);

	bool Double(double d);

	bool String(const char* str, SizeType length, bool copy);

	bool Key(const char* str, SizeType length, bool copy);

	bool StartObject();

	bool EndObject(SizeType memberCount);

	bool StartArray();

	bool EndArray(SizeType elementCount);
};

