#include "Reader.h"
#include "Network.h"
#include "../util/MemoryStream.h"

#define MAX_BUFFER_SIZE (1024 * 1024)

namespace Network {
	Reader::Reader(int size) {
		defaultSize_ = size;
		size_ = size;
		total_ = 0;
		head_ = tail_ = freelist_ = NULL;
	}

	Reader::~Reader() {
		ReaderBuffer* tmp = NULL;
		while ((tmp = head_) != NULL) {
			head_ = head_->next_;
			delete tmp;
		}
		while ((tmp = freelist_) != NULL) {
			freelist_ = freelist_->next_;
			delete tmp;
		}
	}

	int Reader::Read(int fd) {
		int total = 0;
	
		ReaderBuffer* buffer = NextBuffer();
		int left = buffer->size_ - buffer->wpos_;
		int len = Network::SocketRead(fd,buffer->data_ + buffer->wpos_,left);
		if (len > 0) {
			buffer->wpos_ += len;
			total_ += len;
			total += len;
			if (len < left) {
				size_ /= 2;
				if (size_ < defaultSize_) {
					size_ = defaultSize_;
				}
			} else {
				size_ *= 2;
				if (size_ > MAX_BUFFER_SIZE) {
					size_ = MAX_BUFFER_SIZE;
				}
			}
		} else if (len == 0) {
			return 0;
		} else {
			return -1;
		}

		return total;
	}

	void Reader::ReadData(char* data,int size) {
		assert(size <= total_);
		int offset = 0;
		while (offset < size) {
			ReaderBuffer* buffer = head_;

			int left = size - offset;
			if (buffer->wpos_ - buffer->rpos_ >= left) {
				memcpy(data + offset,buffer->data_ + buffer->rpos_,left);
				buffer->rpos_ += left;
				offset += left;
				total_ -= left;

				if (buffer->wpos_ - buffer->rpos_ == 0)
					FreeHead();
			} else {
				int readable = buffer->wpos_ - buffer->rpos_;
				memcpy(data + offset,buffer->data_ + buffer->rpos_,readable);
				buffer->rpos_ += readable;
				offset += readable;
				total_ -= readable;

				FreeHead();
			}
		}
	}

	int Reader::GetLength() {
		return total_;
	}

	Reader::ReaderBuffer* Reader::NextBuffer() {
		ReaderBuffer* buffer = NULL;
		if (tail_ ==  NULL) {
			assert(head_ == NULL);
			buffer = AllocBuffer();
			head_ = tail_ = buffer;
		} else {
			if (tail_->size_ - tail_->wpos_ == 0) {
				buffer = AllocBuffer();
				tail_->next_ = buffer;
				tail_ = buffer;
				buffer->next_ = NULL;
			} else {
				buffer = tail_;	
			}
		}
		return buffer;
	}

	Reader::ReaderBuffer* Reader::AllocBuffer() {
		ReaderBuffer* buffer = NULL;
		if (freelist_ == NULL) {
			freelist_ = new ReaderBuffer(size_);
		}
		buffer = freelist_;
		freelist_ = freelist_->next_;
		buffer->next_ = NULL;
		return buffer;
	}

	void Reader::FreeHead() {
		ReaderBuffer* tmp = head_;

		head_ = head_->next_;

		tmp->next_ = freelist_;
		freelist_ = tmp;
		tmp->rpos_ = tmp->wpos_ = 0;

		if (head_ == NULL) {
			tail_ = NULL;
		}
	}

}