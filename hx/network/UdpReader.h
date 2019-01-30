#ifndef UDPREADER_H
#define UDPREADER_H

#include "util/RingBuffer.h"
#include "network/Reader.h"

namespace Network {
	class UdpReader : public Reader {
	public:
		UdpReader();
		virtual ~UdpReader();

		virtual int  Read(int fd);
		virtual void ReadData(char* data, int size);

		virtual int GetTotal();

	private:

		RingBuffer* buff_;
	}
#endif