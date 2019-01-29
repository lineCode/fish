#ifndef RINGBUFFER_H
#define RINGBUFFER_H


class RingBuffer {
public:
	RingBuffer(uint32_t min, uint32_t max);
	virtual ~RingBuffer();


private:
	char* buffer_;
	uint32_t size_;
	uint32_t max_;
	uint32_t readOffset_;
	uint32_t writeOffset_;
};

#endif