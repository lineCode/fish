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

		WriterBuffer() {
			Reset();
		}

		~WriterBuffer() {
		}

		void* Begin() {
			return (void*)( (char*)data_ + offset_ );
		}

		void Skip(int offset) {
			offset_ += offset;
		}

		void Reset() {
			data_ = NULL;
			size_ = 0;
			offset_ = 0;
			reference_ = NULL;
			next_ = NULL;
		}

		int Writable() {
			return size_ - offset_;
		}
	};

	class TcpWriter : public Writer {
	
	public:
		TcpWriter();

		virtual ~TcpWriter();

		virtual int Write(int fd);

		virtual void Append(void* data, int size, uint32_t* reference);

	private:

		WriterBuffer* AllocBuffer();

		void Remove();

		void FreeBuffer(WriterBuffer* buffer);

	private:
		WriterBuffer* head_;
		WriterBuffer* tail_;
		WriterBuffer* freelist_;
	};

};
#endif
