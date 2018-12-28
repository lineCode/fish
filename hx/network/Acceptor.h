#ifndef ACCEPTOR_H
#define ACCEPTOR_H

#include "EventPoller.h"
#include "ev++.h"
#include "Address.h"
#include <iostream>
#include <functional>


namespace Network {
	class Acceptor {
		public:
			typedef std::function<void(int, Addr&,void*)> OnConnection;

		public:
			Acceptor(EventPoller* poller);

			virtual ~Acceptor();

			void SetCallback(OnConnection callback);

			void SetUserdata(void* userdata);

			void* GetUserdata();
			
			virtual int Listen(Addr& addr);

			virtual int Listen(const char * host,int port);

			virtual int Close();

		private:
			virtual void HandleConnection(ev::io &w, int revents);

			EventPoller* poller_;
			ev::io io_;
			Addr addr_;
			int fd_;
			OnConnection callback_;
			void* userdata_;
	};
}

#endif

