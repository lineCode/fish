#ifndef TCPWRITER_H
#define TCPWRITER_H

#include <stdint.h>

#include "network/Writer.h"

namespace Network {


	struct WriterBuffer {
		int size_;
		int offset_;
		void* data_;
		uint32_t* reference_;
		WriterBuffer* next_;

		WriterBuffer();

		~WriterBuffer();

		void* Begin();

		void Skip(int offset);

		void Reset();

		int Writable();
	};

	class TcpWriter : public Writer {
	
	public:
		TcpWriter();

		virtual ~TcpWriter();

		virtual int Write(int fd);

		virtual void Append(void* data, int size, uint32_t* reference);

	private:
		bool Empty();

		WriterBuffer* AllocBuffer();

		WriterBuffer* Front();

		void RemoveFront();

		void FreeBuffer(WriterBuffer* buffer);

	private:
		WriterBuffer* head_;
		WriterBuffer* tail_;
		WriterBuffer* freelist_;
	};

};
#endif
