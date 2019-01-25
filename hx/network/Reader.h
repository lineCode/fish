﻿#ifndef READER_H
#define READER_H

#include "util/List.h"

namespace Network {
	class Reader {
	public:
		struct ReaderBuffer {
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
		Reader(int size = 64);
		virtual ~Reader();

		virtual int  Read(int fd);
		virtual void ReadData(char* data,int size);

		virtual int GetLength();
	private:
		ReaderBuffer* NextBuffer();
		ReaderBuffer* AllocBuffer();
		void FreeHead();

	public:
		int	total_;

	private:
		int defaultSize_;
		int	size_;
		ReaderBuffer* head_;
		ReaderBuffer* tail_;
		ReaderBuffer* freelist_;
	};
}

#endif
