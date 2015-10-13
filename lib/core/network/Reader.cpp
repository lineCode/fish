#include "Reader.h"
#include "Network.h"
#include "../util/MemoryStream.h"

namespace Network
{
	Reader::Reader(Session* session,int buffersize)
	{
		_session = session;
		_buffsize = buffersize;
		_total = 0;
	}

	Reader::~Reader()
	{
		while (_freelist.Empty() == false)
		{
			ReaderBuffer* buffer;
			_freelist.PopHead(buffer);
			delete buffer;
		}

		while(_waitlist.Empty() == false)
		{
			ReaderBuffer* buffer;
			_waitlist.PopHead(buffer);
			delete buffer;
		}
	}

	int Reader::Read(int fd)
	{
		ReaderBuffer* buffer = NULL;
		this->PopBuffer(buffer);

		int len = Network::SocketRead(fd,buffer->WriteBegin(),buffer->Left());
		if (len > 0)
		{
			buffer->_wpos += len;
			_total += len;
		}
		return len;
	}

	void Reader::ReadData(char* data,int size)
	{
		assert(size <= _total);
		int left = size;
		int offset = 0;
		while (left > 0)
		{
			assert(_waitlist.Empty() == false);
			ReaderBuffer* buffer = _waitlist.Front();

			if (buffer->Length() >= left)
			{
				memcpy(data + offset,buffer->ReadBegin(),left);
				buffer->_rpos += left;
				offset += left;
				_total -= left;
				left -= left;

				if (buffer->Length() == 0)
				{
					_waitlist.RemoveFront();
					this->PushBuffer(buffer);
				}
			}
			else
			{
				int readable = buffer->Length();
				memcpy(data + offset,buffer->ReadBegin(),readable);
				buffer->_rpos += readable;
				offset += readable;
				_total -= readable;
				left -= readable;

				if (buffer->Length() == 0)
				{
					_waitlist.RemoveFront();
					this->PushBuffer(buffer);
				}
			}
		}
	}

	void Reader::PopBuffer(ReaderBuffer*& buffer)
	{
		buffer = NULL;
		if (_waitlist.Empty() == false)
		{
			buffer = _waitlist.Back();
			if (buffer->Left() > 0)
				return;
		}

		if (_freelist.Empty())
			buffer = new ReaderBuffer(_buffsize);
		else
			_freelist.PopHead(buffer);
		
		_waitlist.PushTail(buffer);

		return;
	}

	void Reader::PushBuffer(ReaderBuffer* buffer)
	{
		buffer->_rpos = buffer->_wpos = 0;
		_freelist.PushTail(buffer);
	}
}