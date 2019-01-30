#include "FastReader.h"
#include "Network.h"
#include <stdlib.h>
#include <string.h>

namespace Network {

FastReader::FastReader(int size) {
	buff_ = new RingBuffer(size, 2048);
}

FastReader::~FastReader() {
	delete buff_;
}

int FastReader::Read(int fd) {
	int total = 0;
	for ( ;; ) {
		uint32_t space;
		char* input = buff_->PreWrite(space);
		if (!input || space == 0) {
			break;
		}
		int len = Network::SocketTcpRead(fd, input, space);
		if (len > 0) {
			total += len;
			buff_->Commit(len);
		} else if ( len == 0 ) {
			break;
		} else {
			return -1;
		}
	}
	return total;
}

void FastReader::ReadData(char* data, int size) {
	assert(size <= buff_->GetUsedSize());
	char* output = buff_->Read(size);
	memcpy(data, output, size);
}

char* FastReader::PeekData(int size) {
	if (size > buff_->GetUsedSize()) {
		return NULL;
	}
	return buff_->Read(size);
}

int FastReader::GetTotal() {
	return buff_->GetUsedSize();
}

}