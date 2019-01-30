#ifndef TCPREADER_H
#define TCPREADER_H

#include "network/Reader.h"
#include "util/List.h"

namespace Network {
	class TcpReader : public Reader {
	public:
		struct ReaderBuffer {
			int	rpos_;
			int	wpos_;
			int	size_;
			char* data_;
			ReaderBuffer* next_;

			ReaderBuffer(int size) :rpos_(0), wpos_(0), size_(size), next_(NULL)
			{
				data_ = (char*)malloc(size);
			}

			~ReaderBuffer()
			{
				free((void*)data_);
			}
		};

	public:
		TcpReader(int size = 64);
		virtual ~TcpReader();

		virtual int  Read(int fd);
		virtual void ReadData(char* data, int size);
		virtual char* ReadData(int size);

		virtual int GetTotal();

	private:
		ReaderBuffer* NextBuffer();
		ReaderBuffer* AllocBuffer();
		void FreeHead();

	private:
		int defaultSize_;
		int	size_;
		int	total_;
		ReaderBuffer* head_;
		ReaderBuffer* tail_;
		ReaderBuffer* freelist_;
	};
}

#endif
