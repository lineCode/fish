#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "EventPoller.h"
#include <functional>
#include "ev++.h"
#include "Address.h"

namespace Network
{
	class Connector
	{
	public:
		typedef std::function<void(int,const char*,void*)> OnConnect;

	public:
		Connector(EventPoller* poller);
		~Connector(void);

		virtual int Connect(const Addr& addr);

		virtual int Connect(const char * host,int port);

		void SetCallback(OnConnect callback);

		void SetUserdata(void* userdata);

	private:
		virtual void ConnectCallback(ev::io &w, int revents);

		EventPoller* poller_;
		ev::io io;
		Addr addr_;
		OnConnect callback_;
		void* userdata_;
	};
}

#endif

