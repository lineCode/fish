#include "Acceptor.h"
#include "Network.h"

namespace Network {
	Acceptor::Acceptor(EventPoller* poller) :poller_(poller) {
		fd_ = -1;
		callback_ = NULL;
		userdata_ = NULL;
	}

	Acceptor::~Acceptor() {
		Close();
	}

	void Acceptor::SetCallback(OnConnection callback) {
		callback_ = callback;
	}

	void Acceptor::SetUserdata(void* userdata) {
		userdata_ = userdata;
	}

	void* Acceptor::GetUserdata() {
		return userdata_;
	}

	int Acceptor::Listen(Addr& addr) {
		addr_ = addr;

		int fd = SocketBind(addr_);
		if ( fd < 0 ) {
			return -1;
		}

		if ( SocketListen(fd, 64) == -1 ) {
			return -1;
		}

		fd_ = fd;

		io_.set(poller_->GetLoop());

		io_.set<Acceptor, &Acceptor::HandleConnection>(this);

		io_.start(fd_, EV_READ);

		return fd_;
	}

	int Acceptor::Listen(const char * host, int port) {
		Addr addr = Addr::MakeIP4Addr(host,port);
		return Listen(addr);
	}

	int Acceptor::Close() {
		if (io_.is_active()) {
			io_.stop();
			return 0;
		}
		return -1;
	}

	void Acceptor::HandleConnection(ev::io &w, int revents) {
		Addr addr;
		memset(&addr.sockaddr, 0, sizeof(addr.sockaddr)); 
		int fd = SocketAccept(fd_, &addr);
		if (fd < 0) {
			return;
		}

		if ( callback_ == NULL ) {
			SocketClose(fd);
			return;
		}
		callback_(fd, addr, userdata_);
	}
}
