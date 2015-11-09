#include "Session.h"

namespace Network
{
	Session::Session(Network::EventPoller* poller,int fd):_poller(poller),_fd(fd),_sendBuffer()
	{
		_reader = NULL;
		_state = Alive;
	}

	Session::~Session(void)
	{
	}
	
	int Session::HandleInput()
	{
		if (_reader->Read(_fd) < 0)
		{
			_state = Error;
			this->HandleError();
		}
		return 0;
	}

	int Session::HandleOutput()
	{
		if (_state == Error || _state == Invalid)
			return -1;
		
		int left = _sendBuffer.Left();
		if (left != 0)
		{
			int n = Network::SocketWrite(_fd,(char*)_sendBuffer.Begin(),left);
			if (n >= 0) 
			{
				if (n != 0)
				{
					if (n != left)
						_sendBuffer.SetOffset(n);
				}
			}
			else
				_state = Error;
		}
	
		if (_state == Error)
			this->HandleError();
		else
		{
			if (_sendBuffer.Left() == 0)
			{
				_sendBuffer.Reset();
				_poller->DeRegisterWrite(_id,_fd);
				if (_state == Closed)
					this->HandleError();
			}
		}

		return 0;
	}

	int Session::HandleError()
	{	
		this->Clean();
		this->Fina();
		return 0;
	}

	int Session::Clean()
	{
		if (_poller->isRegistered(_id,true))
			_poller->DeRegisterRead(_id,_fd);

		if (_poller->isRegistered(_id,false))
			_poller->DeRegisterWrite(_id,_fd);

		if (_poller->isRegisteredError(_id))
			_poller->DeRegisterError(_id);

		SocketClose(_fd);

		_poller->RetrieveId(_fd,_id);
		
		_state = Invalid;

		return 0;
	}

	int Session::Send(char* data,int size)
	{
		MemoryStream* ms = new MemoryStream(data,size);
		return this->Send(ms);
	}

	int Session::Send(MemoryStream* ms)
	{
		if (!IsAlive())
			return -1;

		if (_poller->isRegistered(_id,false))
			_sendBuffer.Append(ms->data(),ms->length());
		else
		{
			int n = Network::SocketWrite(_fd,ms->data(),ms->length());
			if (n >= 0) 
			{
				if (n == 0)
				{
					_sendBuffer.Append(ms->data(),ms->length());
					_poller->RegisterWrite(_id,_fd,this);
				}
				else
				{
					if (n != (int)ms->length())
					{
						ms->rpos(n);
						_sendBuffer.Append(ms->data(),ms->length());
						_poller->RegisterWrite(_id,_fd,this);
					}
				}
			}
			else
			{
				_state = Error;
				this->HandleError();
				delete ms;
				return -1;
			}
		}
		delete ms;
		return 0;
	}

	int Session::Close()
	{
		if (!IsAlive())
			return -1;

		_state = Closed;

		if (_sendBuffer.Left() == 0)
		{
			this->HandleError();
			return 0;
		}
		return -1;
	}

	void Session::SetId(int id)
	{
		_id = id;
	}

	int Session::GetId()
	{
		return _id;
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
		return _state == Alive;
	}

}
