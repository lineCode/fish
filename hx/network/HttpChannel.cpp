#include "HttpChannel.h"

HttpChannel::HttpChannel(Network::EventPoller* poller,int fd):Channel(poller,fd) {

}

HttpChannel::~HttpChannel() {

}

void HttpChannel::HandleRead() {

}

void HttpChannel::HandleWrite() {

}

void HttpChannel::HandleError() {
	
}