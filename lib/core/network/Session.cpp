#include "Session.h"

namespace Network
{
	Session::Session(Network::EventPoller* poller,int fd):_poller(poller),_fd(fd)
	{
		_reader = NULL;
		_state = Alive;
	}


	Session::~Session(void)
	{
		while(_sendQueue.empty() == false)
		{
			MemoryStream* ms = _sendQueue.front();
			_sendQueue.pop();
			delete ms;
		}
	}
	
	int Session::HandleInput()
	{
		if (_reader->Read(_fd) < 0)
		{
			_state = Invalid;
			this->HandleError();
		}
		return 0;
	}

	int Session::HandleOutput()
	{
		if (_state == Invalid)
			return -1;
	
		while (_sendQueue.empty() == false && _state != Invalid)
		{
			MemoryStream* ms = _sendQueue.front();

			if (ms->length() != 0)
			{
				int n = Network::SocketWrite(_fd,ms->begin(),ms->length());
				if (n >= 0) 
				{
					if (n == 0)
					{
						//send buffer full
						break;
					}
					else
					{
						if (n != (int)ms->length())
						{
							//send buffer full
							ms->rpos(n);
							break;
						}
						else
						{
							//send done,go to next buffer
							_sendQueue.pop();
							delete ms;
						}
					}
				}
				else
				{
					//send error
					_state = Invalid;
					break;
				}
			}
			else
			{
				_sendQueue.pop();
				delete ms;
			}
		}

		if (_state == Invalid)
			this->HandleError();
		else
		{
			if (_sendQueue.empty())
			{
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
		_state = Invalid;
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

		return 0;
	}

	int Session::Send(char* data,int size)
	{
		MemoryStream* ms = new MemoryStream(data,size);
		return this->Send(ms);
	}

	int Session::Send(MemoryStream* ms)
	{
		if (_state == Closed || _state == Invalid)
			return -1;

		if (_poller->isRegistered(_id,false))
		{
			_sendQueue.push(ms);
		}
		else
		{
			int n = Network::SocketWrite(_fd,ms->data(),ms->length());
			if (n >= 0) 
			{
				if (n == 0)
				{
					_sendQueue.push(ms);
					_poller->RegisterWrite(_id,_fd,this);
				}
				else
				{
					if (n != (int)ms->length())
					{
						ms->rpos(n);
						_sendQueue.push(ms);
						_poller->RegisterWrite(_id,_fd,this);
					}
					else
						delete ms;
				}
			}
			else
			{
				_state = Invalid;
				delete ms;
				this->HandleError();
				return -1;
			}
		}

		return 0;
	}

	int Session::Close()
	{
		if (_state == Closed || _state == Invalid)
			return -1;

		_state = Closed;

		if (_sendQueue.empty())
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
