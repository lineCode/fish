#include "LoggerChannel.h"

LoggerChannel::LoggerChannel(Network::EventPoller* poller, int fd):Network::Channel(poller,fd) {

}

LoggerChannel::~LoggerChannel() {

}

void LoggerChannel::HandleClose() {
	if (callback_) callback_(userdata_);
}

void LoggerChannel::HandleError() {
	if (callback_) callback_(userdata_);
}

void LoggerChannel::SetCloseCallback(Callback callback) {
	callback_ = callback;
}

void LoggerChannel::SetUserdata(void* userdata) {
	userdata_ = userdata;
}