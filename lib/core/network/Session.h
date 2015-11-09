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

		struct SendBuffer
		{

			SendBuffer(int size = 1024):_size(size),_roffset(0),_woffset(0)
			{
				_data = malloc(_size);
			}

			~SendBuffer()
			{
				free(_data);
			}

			void Append(void* data,int size)
			{
				Reserve(size);
				memcpy((char*)_data+_woffset,data,size);
				_woffset += size;
			}

			void* Begin()
			{
				return (void*)((char*)_data+_roffset);
			}

			void SetOffset(int off)
			{
				_roffset += off;
			}

			void Reset()
			{
				_woffset = _roffset = 0;
			}

			int Left()
			{
				return _woffset - _roffset;
			}
		private:
			void Reserve(int size)
			{
				if (_woffset+size < _size)
					return;
				int nsize = _woffset + _size * 2;
				if (nsize < size)
					nsize = size;

				_data = (char*)realloc(_data,nsize);
				_size = nsize;
			}

			int _size;
			int _roffset;
			int _woffset;
			void* _data;
		};
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
		SendBuffer				_sendBuffer;
		SessionState			_state;
	};
}


#endif
