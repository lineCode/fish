#ifndef SESSION_H
#define SESSION_H
#include <vector>
#include <map>
#include <queue>

#include "Interface.h"
#include "EventPoller.h"
#include "TcpReader.h"
#include "../ServerApp.h"
#include "../util/MemoryStream.h"



class ServerApp;

namespace Network
{
	class Session:public Network::InputHandler,
				  public Network::ErrorHandler,
				  public Network::OutputHandler
	{	
	public:
		enum SessionState {Alive,Closed,Error,Invalid};
		typedef std::queue<MemoryStream*> SendQueue;
	public:
		Session(Network::EventPoller* poller,int fd);
		virtual ~Session();

		virtual int Init() = 0;

		virtual int Fina() = 0;

		virtual int Forward(const char * ptr,int size) = 0;

		virtual int HandleInput();

		virtual int HandleOutput();

		virtual int HandleError();

		virtual int Clean();

		virtual int Send(char* data,int size);

		virtual int Send(MemoryStream* ms);

		virtual int Close();

		virtual void SetId(int id);
		virtual int  GetId();

		virtual void SetFd(int fd);
		virtual int	 GetFd();

		virtual void SetReader(Reader * reader);

		virtual bool IsAlive();

	protected:
		Network::EventPoller *	_poller;
		int						_id;
		int						_fd;
		Reader*					_reader;
		SendQueue				_sendQueue;
		SessionState			_state;
	};
}


#endif
