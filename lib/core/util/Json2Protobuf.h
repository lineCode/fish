#ifndef JSON2PROTOBUF_H
#define JSON2PROTOBUF_H

#include "rapidjson/reader.h"

#include <stack>
#include <iostream>
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>


using namespace rapidjson;
using namespace std;

struct Json2Protobuf
{
	std::stack<google::protobuf::Message*> stack_;
	google::protobuf::Message*	top_;
	const google::protobuf::FieldDescriptor* nextField_;
	bool start_;

	Json2Protobuf(google::protobuf::Message* msg);

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

#endif