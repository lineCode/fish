#ifndef ACCEPTOR_H
#define ACCEPTOR_H
#include "Interface.h"
#include "EventPoller.h"

namespace Network
{
	template<class SESSION>
	class Acceptor:public InputHandler,public ErrorHandler
	{
	public:
		Acceptor(EventPoller* poller);
		virtual ~Acceptor();

		virtual int		Listen(const char * host,int port);

		virtual int		HandleInput();

		virtual int		HandleError();

		virtual SESSION* MakeSession(int fd);

	protected:
		EventPoller*	_poller;
		std::string		_host;
		int				_port;
		int				_fd;
		int				_id;
	};

	template<class SESSION>
	Acceptor<SESSION>::Acceptor(EventPoller* poller):_poller(poller)
	{
		_fd = -1;
		_id = -1;
	}

	template<class SESSION>
	Acceptor<SESSION>::~Acceptor()
	{
	}

	template<class SESSION>
	int Acceptor<SESSION>::Listen(const char * host,int port)
	{
		_host.assign(host);
		_port = port;

		int fd = SocketBind(_host.c_str(),_port,IPPROTO_TCP);
		if (fd < 0)
			return -1;

		if (SocketListen(fd,100) == -1) 
			return -1;

		_fd = fd;
		int id = _poller->GenId(fd);
		if (id == 0)
		{
			SocketClose(_fd);
			return -1;
		}
		_id = id;
		this->_poller->RegisterRead(id,fd,this);
		this->_poller->RegisterError(id,fd,this);

		return _fd;
	}

	template<class SESSION>
	int Acceptor<SESSION>::HandleInput() 
	{
		struct sockaddr	sa;
		int len = sizeof(sa);
		int rfd = accept(_fd, (sockaddr*)&sa, (socklen_t*)&len);
		
		SocketSetKeepalive(rfd,true);
		SocketSetNodelay(rfd,true);
		SocketSetNonblocking(rfd,true);

		int id = _poller->GenId(rfd);
		if (id != 0)
		{
			SESSION* session = this->MakeSession(rfd);
			session->SetId(id);
			_poller->RegisterRead(id,rfd,session);
			_poller->RegisterError(id,rfd,session);
			session->Init();
		}
		else
			SocketClose(rfd);

		return 0;
	}

	template<class SESSION>
	int Acceptor<SESSION>::HandleError() 
	{
		_poller->RetrieveId(_fd,_id);
		SocketClose(_fd);

		return 0;
	}

	template<class SESSION>
	SESSION* Acceptor<SESSION>::MakeSession(int fd) 
	{
		SESSION * session = new SESSION(_poller,fd);
		return session;
	}
}

#endif

