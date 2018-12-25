#include "Connector.h"
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

	int Connector::Connect(const char * host, int port)
	{
		host_.assign(host);
		port_ = port;

		bool connected = false;
		if ( ( fd_ = SocketConnect(host, port, true, connected) ) < 0 )
			return -1;

		if ( connected )
		{
			successCallback_(fd_);
			return 0;
		}
		else
		{
			io.start(fd_, EV_READ);
		}
		return 0;
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
			successCallback_(fd_);
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

