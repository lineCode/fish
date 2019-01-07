#include "Connector.h"
#include "Network.h"
namespace Network
{

	Connector::Connector(EventPoller* poller) :poller_(poller) {
		io_.set(poller_->GetLoop());
		io_.set<Connector, &Connector::ConnectCallback>(this);
		callback_ = nullptr;
		userdata_ = NULL;
	}

	Connector::~Connector(void) {
		Close();
	}

	int Connector::Connect(const Addr& addr, bool nonblock) {
		if (io_.is_active()) {
			return -1;
		}
		
		addr_ = addr;

		int fd;
		bool connected = false;
		if ( ( fd = SocketConnect(addr_, nonblock, connected) ) < 0 ) {
			return -1;
		}
		
		if (!nonblock) {
			return fd;
		}		

		if ( connected ) {
			callback_(fd, NULL, userdata_);	
		} else {
			io_.start(fd, EV_WRITE);
		}
		return 0;
	}

	int Connector::Connect(const char * host, int port, bool nonblock) {
		Addr addr = Addr::MakeIP4Addr(host, port);
		return Connect(addr,nonblock);
	}

	int Connector::Close() {
		if (io_.is_active()) {
			SocketClose(io_.fd);
			io_.stop();
			return 0;
		}
		return -1;
	}

	void Connector::ConnectCallback(ev::io &w, int revents) {
		io_.stop();

		int error;
		socklen_t len = sizeof( error );
		int code = getsockopt(w.fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
		if ( code < 0 || error ) {
			char* strerr = NULL;
			if ( code >= 0 ) {
				strerr = strerror(error);
			} else {
				strerr = strerror(errno);
			}

			callback_(-1, strerr, userdata_);
		} else {
			callback_(w.fd, NULL, userdata_);
		}
		
	}

	void Connector::SetCallback(OnConnect callback) {
		callback_ = callback;
	}

	void Connector::SetUserdata(void* userdata) {
		userdata_ = userdata;
	}

	void* Connector::GetUserdata() {
		return userdata_;
	}
}

