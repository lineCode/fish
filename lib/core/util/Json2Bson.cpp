#include "Json2Bson.h"

Json2Bson::Json2Bson()
{
	isArray_ = false;
}


bool Json2Bson::Null() 
{ 
	return true; 
}

bool Json2Bson::Bool(bool b)
{ 
	bson::StringData key(field_);
	builder_->append(key,b);
	stackField_.pop();
	field_ = stackField_.top();
	return true; 
}

bool Json2Bson::Int(int i) 
{ 
	bson::StringData key(field_);
	builder_->append(key,i);
	stackField_.pop();
	field_ = stackField_.top();
	return true; 
}

bool Json2Bson::Uint(unsigned u)
{ 
	bson::StringData key(field_);
	builder_->append(key,u);
	stackField_.pop();
	field_ = stackField_.top();
	return true; 
}

bool Json2Bson::Int64(int64_t i) 
{ 
	bson::StringData key(field_);
	builder_->append(key,i);
	stackField_.pop();
	field_ = stackField_.top();
	return true; 
}

bool Json2Bson::Uint64(uint64_t u)
{ 
	bson::StringData key(field_);
	builder_->appendIntOrLL(key,u);
	stackField_.pop();
	field_ = stackField_.top();
	return true; 
}

bool Json2Bson::Double(double d)
{ 
	bson::StringData key(field_);
	builder_->append(key,d);
	stackField_.pop();
	field_ = stackField_.top();
	return true; 
}

bool Json2Bson::String(const char* str, SizeType length, bool copy) 
{ 
	bson::StringData key(field_);
	builder_->append(key,str,length+1);
	stackField_.pop();
	field_ = stackField_.top();
	return true;
}

bool Json2Bson::Key(const char* str, SizeType length, bool copy)
{
	field_.assign(str,length);
	stackField_.push(field_);
	return true;
}

bool Json2Bson::StartObject()
{
	bson::BSONObjBuilder* builder = new bson::BSONObjBuilder();
	stackBuilder_.push(builder);
	builder_ = builder;
	return true;
}

bool Json2Bson::EndObject(SizeType memberCount) 
{
	bson::BSONObj bo = builder_->obj();
	stackBuilder_.pop();
	if (!stackBuilder_.empty())
	{
		delete builder_;
		builder_ = stackBuilder_.top();
		bson::StringData key(field_);

		builder_->appendObject(key,bo.objdata(),bo.objsize());

		stackField_.pop();
		if (stackField_.empty() == false)
			field_ = stackField_.top();
	}
	else
		bsonObj_ = bo;
	return true; 
}

bool Json2Bson::StartArray()
{ 
	isArray_ = true;
	return true; 
}

bool Json2Bson::EndArray(SizeType elementCount) 
{ 
	isArray_ = false;
	return true; 
}