#include "Json2Protobuf.h"


Json2Protobuf::Json2Protobuf(google::protobuf::Message* msg)
{
	stack_.push(msg);
	top_ = msg;
	start_ = false;
}


bool Json2Protobuf::Null() 
{ 
	assert(false);
	return true; 
}

bool Json2Protobuf::Bool(bool b)
{ 
	const google::protobuf::Reflection *ref = top_->GetReflection();
	const bool repeated = nextField_->is_repeated();

	if (repeated)
		ref->AddBool(top_,nextField_,b);
	else
		ref->SetBool(top_,nextField_,b);

	return true; 
}

bool Json2Protobuf::Int(int i) 
{ 
	const google::protobuf::Reflection *ref = top_->GetReflection();
	const bool repeated = nextField_->is_repeated();

	switch(nextField_->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		{
			if (repeated)
				ref->AddInt32(top_,nextField_,i);
			else
				ref->SetInt32(top_,nextField_,i);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		{
			if (repeated)
				ref->AddUInt32(top_,nextField_,i);
			else
				ref->SetUInt32(top_,nextField_,i);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		{
			if (repeated)
				ref->AddInt64(top_,nextField_,i);
			else
				ref->SetInt64(top_,nextField_,i);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		{
			if (repeated)
				ref->AddUInt64(top_,nextField_,i);
			else
				ref->SetUInt64(top_,nextField_,i);
		}
		break;
	default:
		assert(false);
	}
	return true; 
}

bool Json2Protobuf::Uint(unsigned u)
{ 
	const google::protobuf::Reflection *ref = top_->GetReflection();
	const bool repeated = nextField_->is_repeated();

	switch(nextField_->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		{
			if (repeated)
				ref->AddInt32(top_,nextField_,u);
			else
				ref->SetInt32(top_,nextField_,u);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		{
			if (repeated)
				ref->AddUInt32(top_,nextField_,u);
			else
				ref->SetUInt32(top_,nextField_,u);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		{
			if (repeated)
				ref->AddInt64(top_,nextField_,u);
			else
				ref->SetInt64(top_,nextField_,u);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		{
			if (repeated)
				ref->AddUInt64(top_,nextField_,u);
			else
				ref->SetUInt64(top_,nextField_,u);
		}
		break;
	default:
		assert(false);
	}

	return true; 
}

bool Json2Protobuf::Int64(int64_t i) 
{ 
	const google::protobuf::Reflection *ref = top_->GetReflection();
	const bool repeated = nextField_->is_repeated();


	switch(nextField_->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		{
			if (repeated)
				ref->AddInt32(top_,nextField_,i);
			else
				ref->SetInt32(top_,nextField_,i);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		{
			if (repeated)
				ref->AddUInt32(top_,nextField_,i);
			else
				ref->SetUInt32(top_,nextField_,i);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		{
			if (repeated)
				ref->AddInt64(top_,nextField_,i);
			else
				ref->SetInt64(top_,nextField_,i);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		{
			if (repeated)
				ref->AddUInt64(top_,nextField_,i);
			else
				ref->SetUInt64(top_,nextField_,i);
		}
		break;
	default:
		assert(false);
	}
	return true; 
}

bool Json2Protobuf::Uint64(uint64_t u)
{ 
	const google::protobuf::Reflection *ref = top_->GetReflection();
	const bool repeated = nextField_->is_repeated();

	switch(nextField_->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		{
			if (repeated)
				ref->AddInt32(top_,nextField_,u);
			else
				ref->SetInt32(top_,nextField_,u);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		{
			if (repeated)
				ref->AddUInt32(top_,nextField_,u);
			else
				ref->SetUInt32(top_,nextField_,u);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		{
			if (repeated)
				ref->AddInt64(top_,nextField_,u);
			else
				ref->SetInt64(top_,nextField_,u);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		{
			if (repeated)
				ref->AddUInt64(top_,nextField_,u);
			else
				ref->SetUInt64(top_,nextField_,u);
		}
		break;
	default:
		assert(false);
	}
	return true; 
}

bool Json2Protobuf::Double(double d)
{ 
	const google::protobuf::Reflection *ref = top_->GetReflection();
	const bool repeated = nextField_->is_repeated();

	switch(nextField_->cpp_type())
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		{
			if (repeated)
				ref->AddInt32(top_,nextField_,d);
			else
				ref->SetInt32(top_,nextField_,d);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		{
			if (repeated)
				ref->AddUInt32(top_,nextField_,d);
			else
				ref->SetUInt32(top_,nextField_,d);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		{
			if (repeated)
				ref->AddInt64(top_,nextField_,d);
			else
				ref->SetInt64(top_,nextField_,d);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		{
			if (repeated)
				ref->AddUInt64(top_,nextField_,d);
			else
				ref->SetUInt64(top_,nextField_,d);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		{
			if (repeated)
				ref->AddFloat(top_,nextField_,d);
			else
				ref->SetFloat(top_,nextField_,d);
		}
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		{
			if (repeated)
				ref->AddDouble(top_,nextField_,d);
			else
				ref->SetDouble(top_,nextField_,d);
		}
		break;
	default:
		assert(false);
	}

	return true; 
}

bool Json2Protobuf::String(const char* str, SizeType length, bool copy) 
{ 
	const google::protobuf::Reflection *ref = top_->GetReflection();
	const bool repeated = nextField_->is_repeated();

	if (nextField_->type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
	{
	}
	else
	{
		if (repeated)
			ref->AddString(top_,nextField_,str);
		else
			ref->SetString(top_,nextField_,str); 
	}
	return true;
}

bool Json2Protobuf::Key(const char* str, SizeType length, bool copy)
{
	const google::protobuf::Descriptor *desc = top_->GetDescriptor();
	const google::protobuf::Reflection *ref = top_->GetReflection();

	nextField_ = desc->FindFieldByName(str);
	if (!nextField_)
		nextField_ = ref->FindKnownExtensionByName(str);
	assert(nextField_);

	return true;
}

bool Json2Protobuf::StartObject()
{
	if (start_ == false)
	{
		start_ = true;
		return true;
	}

	const google::protobuf::Reflection *ref = top_->GetReflection();
	const bool repeated = nextField_->is_repeated();

	google::protobuf::Message* subMessage = NULL;
	if (repeated)
		subMessage = ref->AddMessage(top_,nextField_);
	else
		subMessage = ref->MutableMessage(top_,nextField_);

	stack_.push(subMessage);
	top_ = subMessage;
	return true;
}

bool Json2Protobuf::EndObject(SizeType memberCount) 
{
	stack_.pop();
	if (!stack_.empty())
		top_ = stack_.top();
	return true; 
}

bool Json2Protobuf::StartArray()
{ 
	return true; 
}

bool Json2Protobuf::EndArray(SizeType elementCount) 
{ 
	return true; 
}