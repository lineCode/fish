#ifndef WRITER_H
#define WRITER_H

namespace Network {


struct WriterBuffer {
	int size_;
	int offset_;
	void* data_;
	WriterBuffer* next_;

	WriterBuffer();

	~WriterBuffer();

	void* Begin();

	void Skip(int offset);

	void Reset();

	int Writable();
};

struct Writer {
	WriterBuffer* head_;
	WriterBuffer* tail_;
	WriterBuffer* freelist_;

	Writer();

	~Writer();

	int Write(int fd);
	
	bool Empty();

	WriterBuffer* AllocBuffer();

	WriterBuffer* Front();

	void RemoveFront();

	void FreeBuffer(WriterBuffer* buffer);

	void Append(void* data,int size);
};

};
#endif