#include "Session.h"

namespace Network
{
	Session::Session(Network::EventPoller* poller,int fd,int buffersize):poller_(poller),fd_(fd),sendlist_(buffersize)
	{
		reader_ = NULL;
		state_ = Alive;

		rio_.set(poller_->GetEvLoop());
		rio_.set<Session, &Session::HandleInput>(this);

		wio_.set(poller_->GetEvLoop());
		wio_.set<Session, &Session::HandleOutput>(this);
	}

	Session::~Session(void)
	{
	}
	
	void Session::EnableRead() 
	{
		if (!rio_.is_active())
			rio_.start(fd_, EV_READ);
	}

	void Session::DisableRead() 
	{
		if (rio_.is_active())
			rio_.stop(fd_, EV_READ);
	}

	void Session::EnableWrite() 
	{
		if (!rio_.is_active())
			rio_.start(fd_, EV_WRITE);
	}

	void Session::DisableWrite()
	{
		if (wio_.is_active())
			wio_.stop(fd_, EV_WRITE);
	}

	void Session::HandleInput(ev::io &rio, int revents)
	{
		if (_reader->Read(fd_) < 0)
		{
			state_ = Error;
			this->HandleError();
		}
		return 0;
	}

	void Session::HandleOutput(ev::io &wio, int wevents)
	{
		if (state_ == Error || state_ == Invalid)
			return -1;
		
		int result = this->DoSend();
		if (result == 0)
		{
			DisableWrite();
			if (state_ == Closed)
				this->HandleError();
		}
		else if (result < 0)
		{
			state_ = Error;
			this->HandleError();
		}

		return 0;
	}

	void Session::HandleError()
	{	
		DisableRead();
		DisableWrite();

		Clean();
		Fina();
	}

	void Session::Clean()
	{
		DisableRead();
		DisableWrite();

		SocketClose(fd_);
		
		state_ = Invalid;

		return 0;
	}
	
	void Session::Close()
	{
		if (!IsAlive())
			return ;

		state_ = Closed;

		if (sendlist_.Empty())
			this->HandleError();
	}

	int Session::DoSend()
	{
		SendBuffer* buffer = NULL;
		while ((buffer = sendlist_.Front()) != NULL)
		{
			int n = Network::SocketWrite(_fd,(const char*)buffer->Begin(),buffer->Readable());
			if (n >= 0) 
			{
				if (n == buffer->Readable())
					sendlist_.RemoveFront();
				else
				{
					buffer->SetOffset(n);
					return 1;
				}
			}
			else
				return -1;
		}
		return 0;
	}

	int Session::TrySend()
	{
		if (!IsAlive())
			return -1;

		if (!wio_.is_active())
		{
			int result = this->DoSend();
			if (result < 0)
			{
				state_ = Error;
				this->HandleError();
				return -1;
			}
			else if (result == 1)
				EnableWrite();
		}
		return 0;
	}

	int Session::Send(char* data,int size)
	{
		if (!IsAlive())
			return -1;

		sendlist_.Append(data,size);
		return this->TrySend();
	}

	int Session::Send(MemoryStream* ms)
	{
		int result = this->Send(ms->data(),ms->length());
		delete ms;
		return result;
	}

	void Session::SetFd(int fd)
	{
		_fd = fd;
	}

	int Session::GetFd()
	{
		return _fd;
	}

	void Session::SetReader(Reader * reader)
	{
		_reader = reader;
	}

	bool Session::IsAlive()
	{
		return state_ == Alive;
	}

}
