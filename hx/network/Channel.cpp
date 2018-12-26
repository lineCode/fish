#include "Channel.h"
#include "Network.h"
namespace Network
{
	Channel::Channel(Network::EventPoller* poller, int fd) :poller_(poller), fd_(fd), sendlist_()
	{
		reader_ = NULL;
		state_ = Alive;

		rio_.set(poller_->GetEvLoop());
		rio_.set<Channel, &Channel::OnRead>(this);

		wio_.set(poller_->GetEvLoop());
		wio_.set<Channel, &Channel::OnWrite>(this);
	}

	Channel::~Channel(void)
	{
	}
	
	void Channel::EnableRead()
	{
		if (reader_ == NULL)
			return;
		
		if (!rio_.is_active())
			rio_.start(fd_, EV_READ);
	}

	void Channel::DisableRead()
	{
		if (rio_.is_active())
			rio_.stop();
	}

	void Channel::EnableWrite()
	{
		if (!rio_.is_active())
			rio_.start(fd_, EV_WRITE);
	}

	void Channel::DisableWrite()
	{
		if (wio_.is_active())
			wio_.stop();
	}

	void Channel::OnRead(ev::io &rio, int revents)
	{
		assert(reader_ != NULL);

		if (reader_->Read(fd_) < 0)
		{
			state_ = Error;
			Clean();
			this->HandleError();
		}
		else 
		{
			this->HandleRead();
		}
	}

	void Channel::OnWrite(ev::io &wio, int wevents)
	{
		if (state_ == Error || state_ == Invalid)
			return;
		
		int result = this->DoWrite();
		if (result == 0)
		{
			DisableWrite();
			if (state_ == Closed) {
				Clean();
			} else {
				this->HandleWrite();
			}
		}
		else if (result < 0)
		{
			state_ = Error;
			Clean();
			this->HandleError();
		}
	}

	void Channel::HandleRead()
	{
	}

	void Channel::HandleWrite()
	{
	}

	void Channel::HandleError()
	{	
	}

	void Channel::Clean()
	{
		DisableRead();
		DisableWrite();

		SocketClose(fd_);
		
		state_ = Invalid;
	}
	
	void Channel::Close(bool rightnow)
	{
		if (!IsAlive())
			return ;

		state_ = Closed;
		if (!rightnow) {
			EnableWrite();
		} else {
			Clean();
		}
	}

	int Channel::DoWrite()
	{
		SendBuffer* buffer = NULL;
		while ((buffer = sendlist_.Front()) != NULL)
		{
			int n = Network::SocketWrite(fd_,(const char*)buffer->Begin(),buffer->Readable());
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

	int Channel::TryWrite()
	{
		if (!IsAlive())
			return -1;

		if (!wio_.is_active())
		{
			int result = this->DoWrite();
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

	int Channel::Write(char* data, int size)
	{
		if (!IsAlive())
			return -1;

		sendlist_.Append(data,size);
		return this->TrySend();
	}

	int Channel::Write(MemoryStream* ms)
	{
		int result = Write(ms->data(),ms->length());
		delete ms;
		return result;
	}

	void Channel::SetReader(Reader * reader)
	{
		reader_ = reader;
	}

	bool Channel::IsAlive()
	{
		return state_ == Alive;
	}

}
