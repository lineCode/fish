#include "TcpWriter.h"
#include "Network.h"

namespace Network {

	WriterBuffer::WriterBuffer() :size_(0), offset_(0), data_(NULL), reference_(NULL), next_(NULL) {
	}

	WriterBuffer::~WriterBuffer() {
	}

	void* WriterBuffer::Begin() {
		return (void*)( (char*)data_ + offset_ );
	}

	void WriterBuffer::Skip(int offset) {
		offset_ += offset;
	}

	void WriterBuffer::Reset() {
		data_ = NULL;
		size_ = 0;
		offset_ = 0;
		next_ = NULL;
	}
	int WriterBuffer::Writable() {
		return size_ - offset_;
	}

	TcpWriter::TcpWriter() {
		head_ = tail_ = freelist_ = NULL;
	}

	TcpWriter::~TcpWriter() {
		WriterBuffer* wb = NULL;
		while ( ( wb = head_ ) != NULL ) {
			if ( !wb->reference_ ) {
				free(wb->data_);
			}
			else {
				if ( ( --( *wb->reference_ ) ) == 0 ) {
					free(wb->data_);
					free(wb->reference_);
				}
			}
			head_ = wb->next_;
			delete wb;
		}
		wb = NULL;
		while ( ( wb = freelist_ ) != NULL ) {
			freelist_ = wb->next_;
			delete wb;
		}
	}

	int TcpWriter::Write(int fd) {
		WriterBuffer* wb = NULL;
		while ( ( wb = Front() ) != NULL ) {
			int n = Network::SocketWrite(fd, (const char*)wb->Begin(), wb->Writable());
			if ( n >= 0 ) {
				if ( n == wb->Writable() ) {
					RemoveFront();
				}
				else {
					wb->Skip(n);
					return 1;
				}
			}
			else {
				return -1;
			}
		}
		return 0;
	}

	void TcpWriter::Append(void* data, int size, uint32_t* reference) {
		WriterBuffer* wb = AllocBuffer();
		wb->data_ = data;
		wb->size_ = size;
		wb->reference_ = reference;

		if ( tail_ == NULL ) {
			assert(head_ == NULL);
			head_ = tail_ = wb;
		}
		else {
			tail_->next_ = wb;
			wb = tail_;
		}
	}

	bool TcpWriter::Empty() {
		return head_ == NULL;
	}

	WriterBuffer* TcpWriter::AllocBuffer() {
		if ( freelist_ == NULL ) {
			WriterBuffer* buffer = new WriterBuffer();
			freelist_ = buffer;
		}
		WriterBuffer* buffer = freelist_;
		freelist_ = buffer->next_;
		buffer->next_ = NULL;
		return buffer;
	}

	WriterBuffer* TcpWriter::Front() {
		return head_;
	}

	void TcpWriter::RemoveFront() {
		assert(head_ != NULL);
		WriterBuffer* wb = head_;
		head_ = head_->next_;
		if ( head_ == NULL ) {
			tail_ = NULL;
		}
		if ( !wb->reference_ ) {
			free(wb->data_);
		}
		else {
			if ( ( --( *wb->reference_ ) ) == 0 ) {
				free(wb->data_);
				free(wb->reference_);
			}
		}

		FreeBuffer(wb);
	}

	void TcpWriter::FreeBuffer(WriterBuffer* buffer) {
		buffer->Reset();
		buffer->next_ = freelist_;
		freelist_ = buffer;
	}
};
