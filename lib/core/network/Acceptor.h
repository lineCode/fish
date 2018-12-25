#ifndef ACCEPTOR_H
#define ACCEPTOR_H
#include "Interface.h"
#include "EventPoller.h"
#include "ev++.h"
#include <functional>
#include <io.h>

namespace Network
{
	class Acceptor
	{
	public:
		typedef std::function<void(Acceptor*, int, const char*,int)> OnConnection;

	public:
		Acceptor(EventPoller* poller);

		virtual ~Acceptor();

		void SetCallback(OnConnection Callback);

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

