#ifndef READER_H
#define READER_H

#include "util/List.h"

namespace Network {
	class Reader {
	public:
		Reader() {};
		virtual ~Reader() {};

		virtual int  Read(int fd) = 0;
		virtual void ReadData(char* data,int size) = 0;
		virtual char* PeekData(int size) = 0;

		virtual int GetTotal() = 0;
	};
}

#endif
