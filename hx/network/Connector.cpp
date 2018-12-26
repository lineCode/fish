#include "Connector.h"
#include "Network.h"
namespace Network
{

	Connector::Connector(EventPoller* poller) :poller_(poller)
	{
		io.set(poller_->GetEvLoop());
		io.set<Connector, &Connector::ConnectCallback>(this);
	}

	Connector::~Connector(void)
	{
	}

	int Connector::Connect(const Addr& addr) 
	{
		addr_ = addr;

		int fd;
		bool connected = false;
		if ( ( fd = SocketConnect(addr_, true, connected) ) < 0 )
			return -1;

		if ( connected )
		{
			successCallback_(fd);
			return 0;
		}
		else
		{
			io.start(fd, EV_WRITE);
		}
	}

	int Connector::Connect(const char * host, int port)
	{
		Addr addr = Addr:MakeIP4Addr(host, port);
		return Connect(addr);
	}

	void Connector::ConnectCallback(ev::io &w, int revents)
	{
		io.stop();

		int error;
		socklen_t len = sizeof( error );
		int code = getsockopt(w.fd, SOL_SOCKET, SO_ERROR, (char*)&error, &len);
		if ( code < 0 || error ) {
			char* strerr;
			if ( code >= 0 )
				strerr = strerror(error);
			else
				strerr = strerror(errno);

			failCallback_();
		}
		else
		{
			successCallback_(fd);
		}
		
	}

	void Connector::SetSuccessCallback(OnConnectSuccess callback)
	{
		successCallback_ = callback;
	}

	void Connector::SetFailCallback(OnConnectFail callback)
	{
		failCallback_ = callback;
	}
}

