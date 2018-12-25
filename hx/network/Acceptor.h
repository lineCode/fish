#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "EventPoller.h"
#include "ev++.h"
#include <iostream>
#include <functional>


namespace Network
{
	class Acceptor
	{
	public:
		typedef std::function<void(Acceptor*, int, const char*,int)> OnConnection;

	public:
		Acceptor(EventPoller* poller);

		virtual ~Acceptor();

		void SetCallback(OnConnection callback);

		virtual int	Listen(const char * host,int port);

		virtual void HandleConnection(ev::io &w, int revents);

	protected:
		EventPoller* poller_;
		std::string host_;
		int port_;
		int fd_;
		int id_;
		ev::io io;
		OnConnection callback_;
	};
}

#endif

