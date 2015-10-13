#ifndef READER_H
#define READER_H

#include "../util/List.h"

class MemoryStream;

namespace Network
{
	class Session;
	
	class Reader
	{
	public:
		struct ReaderBuffer
		{
			int _rpos;
			int _wpos;
			int _size;
			char *_data;

			ReaderBuffer(int size):_rpos(0),_wpos(0),_size(size)
			{
				_data = (char*) malloc(size);
			}

			~ReaderBuffer()
			{
				free((void*)_data);
			}

			char* ReadBegin()
			{
				return _data + _rpos;
			}

			char* WriteBegin()
			{
				return _data + _wpos;
			}

			int Length()
			{
				return _wpos - _rpos > 0 ? _wpos - _rpos:0;
			}

			int Left()
			{
				return _size - _wpos;
			}
		};
		typedef List<ReaderBuffer>	WaitList;
		typedef List<ReaderBuffer>	FreeList;
	public:
		Reader(Session* session,int buffersize = 64);

		virtual ~Reader();

		virtual int  Read(int fd);
		virtual void ReadData(char* data,int size);

	private:
		virtual void PopBuffer(ReaderBuffer*& ms);

		virtual void PushBuffer(ReaderBuffer* ms);

	protected:
		Session*	_session;
		int			_total;
		int			_buffsize;
		WaitList	_waitlist;
		FreeList	_freelist;
	};
}

#endif