#ifndef WRITER_H
#define WRITER_H
#include <stdint.h>

namespace Network {
	class Writer {
	public:

		Writer();

		virtual ~Writer();

		virtual int Write(int fd);

		virtual void Append(void* data,int size, uint32_t* reference);
	};
};
#endif
