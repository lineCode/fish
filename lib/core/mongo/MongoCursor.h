#ifndef MONGOCURSOR_H
#define MONGOCURSOR_H
#include "../util/MemoryStream.h"

class MongoCursor
{
public:
	MongoCursor(const char* data,int size):_data(data),_size(size),_offset(0){}

	~MongoCursor(void) {}

	bool	More();

	char*	Next();

	static int BsonSize(const char* data);

private:
	const char* _data;
	int _size;
	int _offset;
};

#endif