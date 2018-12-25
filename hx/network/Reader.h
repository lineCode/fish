#ifndef READER_H
#define READER_H

#include "../util/List.h"

class MemoryStream;

namespace Network
{
	class Channel;
	
	class Reader
	{
	public:
		struct ReaderBuffer
		{
			int	rpos_;
			int	wpos_;
			int	size_;
			char* data_;
			ReaderBuffer* next_;

			ReaderBuffer(int size):rpos_(0),wpos_(0),size_(size),next_(NULL)
			{
				data_ = (char*) malloc(size);
			}

			~ReaderBuffer()
			{
				free((void*)data_);
			}
		};

	public:
		Reader(Channel* channel, int size = 64);
		virtual ~Reader();

		virtual int  Read(int fd);
		virtual void ReadData(char* data,int size);

	private:
		ReaderBuffer* AllocBuffer();
		void FreeHead();

	protected:
		Channel* channel_;
		int	total_;

	private:
		int	size_;
		ReaderBuffer* head_;
		ReaderBuffer* tail_;
		ReaderBuffer* freelist_;
	};
}

#endif