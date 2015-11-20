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
			int	_rpos;
			int	_wpos;
			int	_size;
			char* _data;
			ReaderBuffer* _next;

			ReaderBuffer(int size):_rpos(0),_wpos(0),_size(size),_next(NULL)
			{
				_data = (char*) malloc(size);
			}

			~ReaderBuffer()
			{
				free((void*)_data);
			}
		};

	public:
		Reader(Session* session,int size = 64);
		virtual ~Reader();

		virtual int  Read(int fd);
		virtual void ReadData(char* data,int size);

	private:
		ReaderBuffer* AllocBuffer();
		void FreeHead();

	protected:
		Session*	_session;
		int			_total;

	private:
		int	_size;
		ReaderBuffer* _head;
		ReaderBuffer* _tail;
		ReaderBuffer* _freelist;
	};
}

#endif