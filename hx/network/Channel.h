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

		struct SendBuffer {
			SendBuffer():size_(0),offset_(0),data_(NULL),next_(NULL) {
			}

			~SendBuffer() {
			}

			void* Begin() {
				return (void*)((char*)data_+offset_);
			}

			void Skip(int offset)
			{
				offset_ += offset;
			}

			void Reset() {
				data_ = NULL;
				size_ = 0;
				offset_ = 0;
				next_ = NULL;
			}
			int Writable() {
				return size_ - offset_;
			}

			int size_;
			int offset_;
			void* data_;
			SendBuffer* next_;
		};

		struct SendList {
			SendBuffer* head_;
			SendBuffer* tail_;
			SendBuffer* freelist_;

			SendList() {
				head_ = tail_ = freelist_ = NULL;
			}

			~SendList() {
				SendBuffer* sb = NULL;
				while ((sb = head_) != NULL) {
					free(sb->data_);
					head_ = sb->next_;
					delete sb;
				}
				sb = NULL;
				while ((sb = freelist_) != NULL) {
					freelist_ = sb->next_;
					delete sb;
				}
			}

			bool Empty() {
				return head_ == NULL;
			}

			SendBuffer* AllocBuffer() {
				if (freelist_ == NULL) {
					SendBuffer* buffer = new SendBuffer();
					freelist_ = buffer;
				}
				SendBuffer* buffer = freelist_;
				freelist_ = buffer->next_;
				return buffer;
			}

			SendBuffer* Front() {
				return head_;
			}

			void RemoveFront() {
				assert(head_ != NULL);
				SendBuffer* sb = head_;
				head_ = head_->next_;
				if (head_ == NULL) {
					tail_ = NULL;
				}
				free(sb->data_);
				FreeBuffer(sb);
			}

			void FreeBuffer(SendBuffer* buffer) {
				buffer->Reset();
				buffer->next_ = freelist_;
				freelist_ = buffer;
			}

			void Append(void* data,int size) {
				SendBuffer* sb = AllocBuffer();
				sb->data_ = data;
				sb->size_ = size;

				if (tail_ == NULL) {
					assert(head_ == NULL);
					head_ = tail_ = sb;
				} else {
					tail_->next_ = sb;
					sb = tail_;
				}
			}
		};

	public:
		Channel(Network::EventPoller* poller,int fd);
		virtual ~Channel();

		virtual void Close(bool rightnow = false);

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
