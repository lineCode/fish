#include "UdpReader.h"
#include "network/Network.h"
#ifndef WIN32
#include <sys/socket.h>
#endif

namespace Network {
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
			char* input = buff_->PreWrite(space);
			if (!input || space < 1600) {
				break;
			}
			struct sockaddr_in addr;
			socklen_t addrlen = sizeof(addr);

			int n = SocketUdpRead(fd, input, space, (struct sockaddr*)&addr, &addrlen);
			if (n > 0) {
				recv += n;
				buff_->Commit(n);
			} else if (n == 0) {
				break;
			} else {
				return -1;
			}
		}
		return recv;
	}

	void UdpReader::ReadData(char* data, int size) {

	}

	int UdpReader::GetTotal() {
		return buff_->GetUsedSize();
	}
}
