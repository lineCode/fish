#ifndef CONNECTOR_H
#define CONNECTOR_H

#include "EventPoller.h"
#include <functional>
#include "ev++.h"

namespace Network
{
	class Connector
	{
	public:
		typedef std::function<void(int)> OnConnectSuccess;
		typedef std::function<void()> OnConnectFail;

	public:
		Connector(EventPoller* poller);
		~Connector(void);

		virtual int Connect(const char * host,int port);

		virtual void ConnectCallback(ev::io &w, int revents);

		void SetSuccessCallback(OnConnectSuccess callback);

		void SetFailCallback(OnConnectFail callback);

	protected:
		EventPoller* poller_;
		std::string host_;
		int port_;
		int fd_;
		OnConnectSuccess successCallback_;
		OnConnectFail failCallback_;
		ev::io io;
	};
}

#endif

