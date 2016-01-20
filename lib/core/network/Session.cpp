#include "Session.h"

namespace Network
{
	Session::Session(Network::EventPoller* poller,int fd,int buffersize):_poller(poller),_fd(fd),_sendlist(buffersize)
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
		
		int result = this->DoSend();
		if (result == 0)
		{
			_poller->DeRegisterWrite(_id,_fd);
			if (_state == Closed)
				this->HandleError();
		}
		else if (result < 0)
		{
			_state = Error;
			this->HandleError();
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
			_poller->DeRegisterError(_id,_fd);

		SocketClose(_fd);

		_poller->RetrieveId(_fd,_id);
		
		_state = Invalid;

		return 0;
	}
	
	int Session::Close()
	{
		if (!IsAlive())
			return -1;

		_state = Closed;

		if (_sendlist.Empty())
		{
			this->HandleError();
			return 0;
		}
		return -1;
	}

	int Session::DoSend()
	{
		SendBuffer* buffer = NULL;
		while ((buffer = _sendlist.Front()) != NULL)
		{
			int n = Network::SocketWrite(_fd,(const char*)buffer->Begin(),buffer->Readable());
			if (n >= 0) 
			{
				if (n == buffer->Readable())
					_sendlist.RemoveFront();
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

		if (_poller->isRegistered(_id,false) == false)
		{
			int result = this->DoSend();
			if (result < 0)
			{
				_state = Error;
				this->HandleError();
				return -1;
			}
			else if (result == 1)
				_poller->RegisterWrite(_id,_fd,this);
		}
		return 0;
	}

	int Session::Send(char* data,int size)
	{
		if (!IsAlive())
			return -1;
		_sendlist.Append(data,size);
		return this->TrySend();
	}

	int Session::Send(MemoryStream* ms)
	{
		int result = this->Send(ms->data(),ms->length());
		delete ms;
		return result;
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
