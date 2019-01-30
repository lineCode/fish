#include "TcpReader.h"
#include "Network.h"
#include "util/MemoryStream.h"

#define MAX_BUFFER_SIZE (1024 * 1024)

namespace Network {
	TcpReader::TcpReader(int size) {
		defaultSize_ = size;
		size_ = size;
		total_ = 0;
		head_ = tail_ = freelist_ = NULL;
	}

	TcpReader::~TcpReader() {
		ReaderBuffer* tmp = NULL;
		while ( ( tmp = head_ ) != NULL ) {
			head_ = head_->next_;
			delete tmp;
		}
		while ( ( tmp = freelist_ ) != NULL ) {
			freelist_ = freelist_->next_;
			delete tmp;
		}
	}

	int TcpReader::Read(int fd) {
		int total = 0;
		for ( ;; ) {
			ReaderBuffer* buffer = NextBuffer();
			int left = buffer->size_ - buffer->wpos_;
			int len = Network::SocketTcpRead(fd, buffer->data_ + buffer->wpos_, left);
			if ( len > 0 ) {
				buffer->wpos_ += len;
				total_ += len;
				total += len;
				if ( len < left ) {
					size_ /= 2;
					if ( size_ < defaultSize_ ) {
						size_ = defaultSize_;
					}
					break;
				}
				else {
					size_ *= 2;
					if ( size_ > MAX_BUFFER_SIZE ) {
						size_ = MAX_BUFFER_SIZE;
					}
				}
			}
			else if ( len == 0 ) {
				break;
			}
			else {
				return -1;
			}
		}
		return total;
	}

	void TcpReader::ReadData(char* data, int size) {
		assert(size <= total_);
		int offset = 0;
		while ( offset < size ) {
			ReaderBuffer* buffer = head_;

			int left = size - offset;
			if ( buffer->wpos_ - buffer->rpos_ >= left ) {
				memcpy(data + offset, buffer->data_ + buffer->rpos_, left);
				buffer->rpos_ += left;
				offset += left;
				total_ -= left;

				if ( buffer->wpos_ - buffer->rpos_ == 0 )
					FreeHead();
			}
			else {
				int readable = buffer->wpos_ - buffer->rpos_;
				memcpy(data + offset, buffer->data_ + buffer->rpos_, readable);
				buffer->rpos_ += readable;
				offset += readable;
				total_ -= readable;

				FreeHead();
			}
		}
	}

	char* TcpReader::ReadData(int size) {
		if (size > buff_->GetUsedSize()) {
			return NULL;
		}
		return buff_->Read(size);
	}
	
	int TcpReader::GetTotal() {
		return total_;
	}

	TcpReader::ReaderBuffer* TcpReader::NextBuffer() {
		ReaderBuffer* buffer = NULL;
		if ( tail_ == NULL ) {
			assert(head_ == NULL);
			buffer = AllocBuffer();
			head_ = tail_ = buffer;
		}
		else {
			if ( tail_->size_ - tail_->wpos_ == 0 ) {
				buffer = AllocBuffer();
				tail_->next_ = buffer;
				tail_ = buffer;
				buffer->next_ = NULL;
			}
			else {
				buffer = tail_;
			}
		}
		return buffer;
	}

	TcpReader::ReaderBuffer* TcpReader::AllocBuffer() {
		ReaderBuffer* buffer = NULL;
		if ( freelist_ == NULL ) {
			freelist_ = new ReaderBuffer(size_);
		}
		buffer = freelist_;
		freelist_ = freelist_->next_;
		buffer->next_ = NULL;
		return buffer;
	}

	void TcpReader::FreeHead() {
		ReaderBuffer* tmp = head_;

		head_ = head_->next_;

		tmp->next_ = freelist_;
		freelist_ = tmp;
		tmp->rpos_ = tmp->wpos_ = 0;

		if ( head_ == NULL ) {
			tail_ = NULL;
		}
	}

}