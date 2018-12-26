#ifndef CHANNEL_H
#define CHANNEL_H
#include <vector>
#include <map>
#include <queue>


#include "EventPoller.h"
#include "TcpReader.h"
#include "../ServerApp.h"
#include "../util/MemoryStream.h"



class ServerApp;

namespace Network
{
	class Channel
	{	
	public:
		enum eChannelState {Alive,Closed,Error,Invalid};

		struct SendBuffer
		{
			SendBuffer(int size = 1024):size_(size),roff_(0),woff_(0),next_(NULL)
			{
				data_ = malloc(size_);
			}

			~SendBuffer()
			{
				free(data_);
			}

			void Append(void* data,int size)
			{
				Reserve(size);
				memcpy((char*)data_+woff_,data,size);
				woff_ += size;
			}

			void* Begin()
			{
				return (void*)((char*)data_+roff_);
			}

			void SetOffset(int off)
			{
				roff_ += off;
			}

			void Reset()
			{
				woff_ = roff_ = 0;
				next_ = NULL;
			}

			int Readable()
			{
				return woff_ - roff_;
			}

			int Writable()
			{
				return size_ - woff_;
			}

			void Reserve(int size)
			{
				if (woff_+size <= size_)
					return;
				int nsize = woff_ + size_ * 2;
				if (nsize < size)
					nsize = size;
				data_ = (char*)realloc(data_,nsize);
				size_ = nsize;
			}

			int			size_;
			int			roff_;
			int			woff_;
			void*		data_;
			SendBuffer* next_;
		};

		struct SendList
		{
			int size_;
			SendBuffer* head_;
			SendBuffer* tail_;
			SendBuffer* freelist_;

			SendList(int size = 1024)
			{
				size_ = size;
				head_ = tail_ = freelist_ = NULL;
			}

			~SendList()
			{
				SendBuffer* cur = NULL;
				while ((cur = head_) != NULL)
				{
					head_ = cur->next_;
					delete cur;
				}
				cur = NULL;
				while ((cur = freelist_) != NULL)
				{
					freelist_ = cur->next_;
					delete cur;
				}
			}

			bool Empty()
			{
				return head_ == NULL;
			}

			SendBuffer* AllocBuffer()
			{
				if (freelist_ == NULL)
				{
					SendBuffer* buffer = new SendBuffer(size_);
					freelist_ = buffer;
				}
				SendBuffer* cur = freelist_;
				freelist_ = cur->next_;
				cur->Reset();
				return cur;
			}

			SendBuffer* Front()
			{
				return head_;
			}

			void RemoveFront()
			{
				assert(head_ != NULL);
				SendBuffer* cur = head_;
				head_ = head_->next_;
				if (head_ == NULL)
					tail_ = NULL;
				FreeBuffer(cur);
			}

			void FreeBuffer(SendBuffer* buffer)
			{
				buffer->next_ = freelist_;
				freelist_ = buffer;
			}

			void Append(void* data,int size)
			{
				int offset = 0;
				if (tail_ == NULL)
				{
					assert(head_ == NULL);
					head_ = tail_ = AllocBuffer();
				}
again:
				int left = tail_->Writable();
				if (left >= size)
				{
					tail_->Append(((char*)data) + offset,size);
					return;
				}
				else
				{
					tail_->Append(((char*)data) + offset,left);
					offset += left;
					size -= left;

					SendBuffer* buffer = AllocBuffer();
					tail_->next_ = buffer;
					tail_ = buffer;
					goto again;
				}
			}
		};

	public:
		Channel(Network::EventPoller* poller,int fd);
		virtual ~Channel();

		virtual void Close(bool rightnow == false);

		virtual void EnableRead();

		virtual void DisableRead();

		virtual void EnableWrite();

		virtual void DisableWrite();

		virtual void HandleRead();

		virtual void HandleWrite();

		virtual void HandleError();

		virtual int Write(char* data,int size);

		virtual int Write(MemoryStream* ms);

		virtual void SetReader(Reader * reader);

		virtual bool IsAlive();

		virtual int Forward(const char * ptr,int size) = 0;

	private:
		virtual void Clean();

		virtual void OnRead(ev::io &rio, int revents);

		virtual void OnWrite(ev::io &wio, int revents);

		virtual int DoWrite();

		virtual int TryWrite();

	protected:
		Network::EventPoller *	poller_;
		int fd_;
		Reader* reader_;
		SendList sendlist_;
		eChannelState state_;
		ev::io rio_;
		ev::io wio_;
	};
}


#endif
