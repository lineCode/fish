#ifndef RINGBUFFER_H
#define RINGBUFFER_H
#include <stdint.h>

class RingBuffer {
public:
	RingBuffer(uint32_t min, uint32_t max);
	virtual ~RingBuffer();

	char* PreWrite(uint32_t& size);

	void Commit(uint32_t size);

	char* Read(uint32_t size);

	bool Realloc();

	uint32_t GetUsedSize() {
		return used_;
	}

	static char* GetCache(uint32_t size);

private:
	char* buff_;
	uint32_t size_;
	uint32_t max_;
	uint32_t head_;
	uint32_t tail_;
	uint32_t used_;

	static char* cache_;
	static uint32_t cacheSize_;
};

#endif