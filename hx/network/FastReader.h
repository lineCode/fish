#ifndef FASTREADER_H
#define FASTREADER_H


#include "network/Reader.h"
#include "util/RingBuffer.h"

namespace Network {
	class FastReader : public Reader {
	public:
		FastReader(int size = 64, int max = 0);
		virtual ~FastReader();

		virtual int  Read(int fd);
		virtual void ReadData(char* data, int size);
		virtual char* ReadData(int size);

		virtual int GetTotal();

	private:
		RingBuffer* buff_;
	};
}


#endif