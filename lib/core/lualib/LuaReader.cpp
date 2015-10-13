#include "LuaReader.h"
#include "../network/Network.h"
#include "../network/Session.h"

LuaReader::LuaReader(Network::Session* session,int size):Network::Reader(session,size)
{

}

LuaReader::~LuaReader()
{

}

int LuaReader::Read(int fd)
{
	int len = Reader::Read(fd);
	if (len > 0)
	{
		int size = _total;
		char* data = (char*)malloc(size);
		this->ReadData(data,_total);
		this->_session->Forward(data,size);
	}

	return len >= 0 ? len:-1;
}

