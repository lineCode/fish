#ifndef TCPREADER_H
#define TCPREADER_H
#include "Reader.h"

namespace Network
{
	class Session;
	class TcpReader:public Reader
	{
	public:
		enum State{Header,Body};

	public:
		TcpReader(Session* session,int header = 2,int buffersize = 64);
		~TcpReader(void);

		virtual int		Read(int fd);

		virtual int		ReadHeader();

	protected:
		int		_header;
		int		_left;
		State	_state;
	};
}



#endif