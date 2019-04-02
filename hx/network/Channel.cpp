#include "Channel.h"
#include "Network.h"
#include "Writer.h"

namespace Network {
	Channel::Channel(Network::EventPoller* poller, int fd) :poller_(poller), fd_(fd) {
		state_ = eAlive;

		SocketSetNonblocking(fd, true);

		rio_.set(poller_->GetLoop());
		rio_.set<Channel, &Channel::OnRead>(this);

		wio_.set(poller_->GetLoop());
		wio_.set<Channel, &Channel::OnWrite>(this);

		reader_ = NULL;
		writer_ = NULL;
	}

	Channel::~Channel(void) {
		if (reader_) delete reader_;
		if (writer_) delete writer_;
	}
	
	bool Channel::IsAlive() {
		return state_ == eAlive;
	}

	void Channel::Close(bool rightnow) {
		if (!IsAlive()) {
			return ;
		}

		state_ = eClosed;
		if (!rightnow) {
			EnableWrite();
		} else {
			Clean();
			HandleClose();
		}
	}

	void Channel::EnableRead() {
		if (reader_ == NULL)
			return;
		
		if (!rio_.is_active())
			rio_.start(fd_, EV_READ);
	}

	void Channel::DisableRead() {
		if (rio_.is_active())
			rio_.stop();
	}

	void Channel::EnableWrite() {
		if (!rio_.is_active())
			rio_.start(fd_, EV_WRITE);
	}

	void Channel::DisableWrite() {
		if (wio_.is_active())
			wio_.stop();
	}

	void Channel::SetReader(Reader* reader) {
		reader_ = reader;
	}

	void Channel::SetWriter(Writer* writer) {
		writer_ = writer;
	}

	void Channel::OnRead(ev::io &rio, int revents) {
		if (revents & EV_ERROR) {
			return;
		}

		assert(reader_ != NULL);

		if (reader_->Read(fd_) < 0) {
			Clean();
			this->HandleError();
		} else  {
			this->HandleRead();
		}
	}

	void Channel::OnWrite(ev::io &wio, int wevents) {
		if (wevents & EV_ERROR) {
			return;
		}

		if (state_ == eInvalid)
			return;
		
		int result = writer_->Write(fd_);
		if (result == 0) {
			DisableWrite();
			if (state_ == eClosed) {
				Clean();
				HandleClose();
			} else {
				HandleWrite();
			}
		} else if (result < 0) {
			Clean();
			HandleError();
		}
	}

	void Channel::HandleRead() {
	}

	void Channel::HandleWrite() {
	}

	void Channel::HandleClose() {

	}

	void Channel::HandleError() {	
	}

	void Channel::Clean() {
		DisableRead();
		DisableWrite();

		SocketClose(fd_);
		
		state_ = eInvalid;
	}

	int Channel::Write(char* data, int size, uint32_t* reference) {
		writer_->Append(data, size, reference);
		
		if (!IsAlive()) {
			return -1;
		}

		if (!wio_.is_active()) {
			int result = writer_->Write(fd_);
			if (result < 0) {
				Clean();
				this->HandleError();
				return -1;
			}
			else if (result == 1) {
				EnableWrite();
			}
		}

		return 0;
	}
	
	int Channel::Write(std::string& data, uint32_t* reference) {
		if (data.length() == 0) {
			return -1;
		}

		int size = data.length();
		char* str = (char*)malloc(size);
		memcpy(str, data.c_str(), size);
		return Write(str, size, reference);
	}
}
