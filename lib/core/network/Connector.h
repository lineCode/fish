#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "Interface.h"
#include "EventPoller.h"

namespace Network
{
	template<class SESSION>
	class Connector : public OutputHandler,public ErrorHandler
	{
	public:
		Connector(EventPoller* poller);
		~Connector(void);

		virtual int Connect(const char * host,int port);

		virtual int HandleOutput();

		virtual int HandleError();

		virtual int ConnectDone();

		virtual SESSION* MakeSession(int fd);

		virtual SESSION* GetSession();
	protected:
		EventPoller*	_poller;
		SESSION*		_session;
		std::string		_host;
		int				_port;
		int				_fd;
		int				_id;
	};

	template<class SESSION>
	Connector<SESSION>::Connector(EventPoller* poller):_poller(poller)
	{
	}

	template<class SESSION>
	Connector<SESSION>::~Connector(void)
	{
	}

	template<class SESSION>
	int Connector<SESSION>::Connect(const char * host,int port)
	{
		_host.assign(host);
		_port = port;

		bool connected = false;
		if ((_fd = SocketConnect(host,port,true,connected)) < 0)
			return -1;

		_id = _poller->GenId(_fd);
		if (_id == 0)
		{
			SocketClose(_fd);
			return -1;
		}
		
		if (connected)
		{
			ConnectDone();
			return _fd;
		}
		return -1;
	}

	template<class SESSION>
	int Connector<SESSION>::HandleOutput()
	{
		_poller->DeRegisterWrite(_id,_fd);
		ConnectDone();
		return 0;
	}

	template<class SESSION>
	int Connector<SESSION>::HandleError()
	{
		_poller->DeRegisterWrite(_id,_fd);
		_poller->DeRegisterError(_id);
		_poller->RetrieveId(_fd,_id);
		return 0;
	}

	template<class SESSION>
	int Connector<SESSION>::ConnectDone()
	{
		if (_poller->isRegistered(_id,false))
			_poller->DeRegisterWrite(_id,_fd);

		if (_poller->isRegisteredError(_id))
			_poller->DeRegisterError(_id);

		SocketSetKeepalive(_fd,true);
		SocketSetNonblocking(_fd,true);

		_poller->RetrieveId(_fd,_id);

		int id = _poller->GenId(_fd);
		if (id != 0)
		{
			_session = MakeSession(_fd);
			_session->SetId(id);
			_poller->RegisterRead(id,_fd,_session);
			_poller->RegisterError(id,_session);

			_session->Init();
		}
		else
			SocketClose(_fd);

		return 0;
	}

	template<class SESSION>
	SESSION* Connector<SESSION>::MakeSession(int fd)
	{
		return new SESSION(_poller,_fd);
	}

	template<class SESSION>
	SESSION* Connector<SESSION>::GetSession()
	{
		return _session;
	}
}

#endif

