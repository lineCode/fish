#ifndef WRITER_H
#define WRITER_H
#include <stdint.h>

namespace Network {
	class Writer {
	public:

		Writer() {};

		virtual ~Writer() {};

		virtual int Write(int fd) = 0;

		virtual void Append(void* data,int size, uint32_t* reference) = 0;
	};
};
#endif
