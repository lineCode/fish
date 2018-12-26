#include "Acceptor.h"
#include "Network.h"
namespace Network
{
	Acceptor::Acceptor(EventPoller* poller) :poller_(poller)
	{
		fd_ = -1;
		callback_ = NULL;
		userdata_ = NULL;
	}

	Acceptor::~Acceptor()
	{
	}

	void Acceptor::SetCallback(OnConnection callback)
	{
		callback_ = callback;
	}

	void Acceptor::SetUserData(void* userdata)
	{
		userdata_ = userdata;
	}

	int Acceptor::Listen(Addr& addr)
	{
		addr_ = addr;

		int fd = SocketBind(addr_);
		if ( fd < 0 )
			return -1;

		if ( SocketListen(fd, 64) == -1 )
			return -1;

		fd_ = fd;

		io_.set(poller_->GetEvLoop());

		io_.set<Acceptor, &Acceptor::HandleConnection>(this);

		io_.start(fd_, EV_READ);

		return fd_;

	}

	int Acceptor::Listen(const char * host, int port)
	{
		Addr addr = Addr::MakeIP4Addr(host,port);
		return Listen(addr);
	}

	void Acceptor::HandleConnection(ev::io &w, int revents)
	{
		Addr adddr;
		int fd = SocketAccept(fd_, &adddr);
		if (fd < 0) {
			return;
		}

		if ( callback_ == NULL ) {
			SocketClose(fd);
			return;
		}
		callback_(w.fd, adddr);
	}
}
