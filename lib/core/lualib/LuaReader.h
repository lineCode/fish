#ifndef LUAREADER_H
#define LUAREADER_H

#include "../network/Reader.h"
#include "../util/MemoryStream.h"

class LuaReader : public Network::Reader
{
public:
	LuaReader(Network::Session* session,int size = 64);
	virtual ~LuaReader();

	virtual int Read(int fd);

protected:
	MemoryStream _stream;
};
#endif