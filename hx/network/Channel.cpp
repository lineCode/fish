#include "Channel.h"
#include "Network.h"
#include "Writer.h"

namespace Network {
	Channel::Channel(Network::EventPoller* poller, int fd) :poller_(poller), fd_(fd) {
		state_ = Alive;

		rio_.set(poller_->GetEvLoop());
		rio_.set<Channel, &Channel::OnRead>(this);

		wio_.set(poller_->GetEvLoop());
		wio_.set<Channel, &Channel::OnWrite>(this);

		reader_ = new Reader();
		writer_ = new Writer();
	}

	Channel::~Channel(void) {
		delete reader_;
		delete writer_;
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

	void Channel::OnRead(ev::io &rio, int revents) {
		assert(reader_ != NULL);

		if (reader_->Read(fd_) < 0) {
			state_ = Error;
			Clean();
			this->HandleError();
		} else  {
			this->HandleRead();
		}
	}

	void Channel::OnWrite(ev::io &wio, int wevents) {
		if (state_ == Error || state_ == Invalid)
			return;
		
		int result = writer_->Write(fd_);
		if (result == 0) {
			DisableWrite();
			if (state_ == Closed) {
				Clean();
				HandleClose();
			} else {
				HandleWrite();
			}
		} else if (result < 0) {
			state_ = Error;
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
		
		state_ = Invalid;
	}
	
	void Channel::Close(bool rightnow) {
		if (!IsAlive()) {
			return ;
		}

		state_ = Closed;
		if (!rightnow) {
			EnableWrite();
		} else {
			Clean();
			HandleClose();
		}
	}

	int Channel::Write(char* data, int size) {
		if (!IsAlive()) {
			return -1;
		}

		writer_->Append(data, size);

		if (!wio_.is_active()) {
			int result = writer_->Write(fd_);
			if (result < 0) {
				state_ = Error;
				this->HandleError();
				return -1;
			}
			else if (result == 1) {
				EnableWrite();
			}
		}

		return 0;
	}

	int Channel::Write(MemoryStream* ms) {
		int result = Write(ms->data(),ms->length());
		delete ms;
		return result;
	}

	bool Channel::IsAlive() {
		return state_ == Alive;
	}

}
