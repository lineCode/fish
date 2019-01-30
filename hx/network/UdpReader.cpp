#include "UdpReader.h"

UdpReader::UdpReader() {
	buff_ = new RingBuffer(1600, 4096);
}

UdpReader::~UdpReader() {
	delete buff_;
}

int UdpReader::Read(int fd) {
	int recv = 0;
	while (true) {
		uint32_t space;
		char* input = buff_->PrePush(space);
		if (!input || space < 1600) {
			break;
		}

		int n = recvfrom(fd, input, space, 0, )
	}
	return recv;
}

void UdpReader::ReadData(char* data, int size) {

}

int UdpReader::GetTotal() {
	return buff_->GetUsedSize();
}