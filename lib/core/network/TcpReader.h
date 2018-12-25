#ifndef TCPREADER_H
#define TCPREADER_H
#include "Reader.h"

namespace Network
{
	class Channel;
	class TcpReader:public Reader
	{
	public:
		enum State{Header,Body};

	public:
		TcpReader(Channel* channel, int header = 2, int buffersize = 64);
		~TcpReader(void);

		virtual int		Read(int fd);

	protected:
		int header_;
		int left_;
		State state_;
	};
}



#endif