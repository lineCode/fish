#include "MongoCursor.h"
#include "bson.h"


bool MongoCursor::More()
{
	int total = _size - _offset;
	if (total == 0)
		return false;

	int left = total - MongoCursor::BsonSize(_data);
	if (left >= 0)
		return true;
	return false;
}

char* MongoCursor::Next()
{
	char* data = (char*)_data + _offset;
	_offset += MongoCursor::BsonSize(data);
	return data;
}

int MongoCursor::BsonSize(const char* data)
{
	assert(data != NULL);
	return *(reinterpret_cast<const int*>(data));
}
