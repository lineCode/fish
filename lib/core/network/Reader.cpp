#include "Reader.h"
#include "Network.h"
#include "../util/MemoryStream.h"

namespace Network
{
	Reader::Reader(Session* session,int size)
	{
		_session = session;
		_size = size;
		_total = 0;
		_head = _tail = _freelist;
	}

	Reader::~Reader()
	{
		ReaderBuffer* tmp = NULL;
		while ((tmp = _head) != NULL)
		{
			_head = _head->_next;
			delete tmp;
		}
		while ((tmp = _freelist) != NULL)
		{
			_freelist = _freelist->_next;
			delete tmp;
		}
	}

	int Reader::Read(int fd)
	{
		ReaderBuffer* buffer = NULL;
		if (_tail ==  NULL)
		{
			assert(_head == NULL);
			_head = _tail = new ReaderBuffer(_size);
			_head->_next = _tail;
			buffer = _tail;
		}
		else
		{
			if (_tail->_size - _tail->_wpos == 0)
			{
				if (_freelist == NULL)
					_freelist = new ReaderBuffer(_size);
				buffer = _freelist;
				_freelist = _freelist->_next;
				_tail->_next = buffer;
				_tail = buffer;
				buffer->_next = NULL;
			}
			else
				buffer = _tail;	
		}
		

		int len = Network::SocketRead(fd,buffer->_data + buffer->_wpos,buffer->_size - buffer->_wpos);
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
		int offset = 0;
		while (offset < size)
		{
			ReaderBuffer* buffer = _head;

			if (buffer->_wpos - buffer->_rpos >= size)
			{
				memcpy(data + offset,buffer->_data + buffer->_rpos,size);
				buffer->_rpos += size;
				offset += size;
				_total -= size;

				if (buffer->_wpos - buffer->_rpos == 0)
				{
					ReaderBuffer* tmp = _head;
					_head = _head->_next;
					tmp->_next = _freelist;
					_freelist = tmp;
					tmp->_rpos = tmp->_wpos = 0;
				}
			}
			else
			{
				int readable = buffer->_wpos - buffer->_rpos;
				memcpy(data + offset,buffer->_data + buffer->_rpos,readable);
				buffer->_rpos += readable;
				offset += readable;
				_total -= readable;

				ReaderBuffer* tmp = _head;
				_head = _head->_next;
				tmp->_next = _freelist;
				_freelist = tmp;
				tmp->_rpos = tmp->_wpos = 0;
			}
		}
	}
}