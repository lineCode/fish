#include "Acceptor.h"
#include "Network.h"
namespace Network
{
	Acceptor::Acceptor(EventPoller* poller) :poller_(poller)
	{

	}

	Acceptor::~Acceptor()
	{
	}

	void Acceptor::SetCallback(OnConnection callback)
	{
		callback_ = callback;
	}

	int Acceptor::Listen(const char * host, int port)
	{
		host_.assign(host);
		port_ = port;

		int fd = SocketBind(host_.c_str(), port_, IPPROTO_TCP);
		if ( fd < 0 )
			return -1;

		if ( SocketListen(fd, 100) == -1 )
			return -1;

		fd_ = fd;

		io.set(poller_->GetEvLoop());

		io.set<Acceptor, &Acceptor::HandleConnection>(this);

		io.start(fd_, EV_READ);

		return fd_;
	}

	void Acceptor::HandleConnection(ev::io &w, int revents)
	{
		if ( callback_ == NULL ) {
			_close(w.fd);
			return;
		}
		callback_(this, w.fd, NULL, 0);
	}
}