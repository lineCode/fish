#include "LuaServerChannel.h"

LuaServerChannel::LuaServerChannel(Network::EventPoller* poller, int fd, int reference) : Channel(poller, fd) {
	reference_ = reference;
}

LuaServerChannel::~LuaServerChannel() {

}

void LuaServerChannel::HandleRead() {
	dataCallback_(reference_, dataRefrence_, NULL, 0);
}

void LuaServerChannel::HandleClose() {
	errorCallback_(reference_, errorRefrence_, "closed");
}

void LuaServerChannel::HandleError() {
	errorCallback_(reference_, errorRefrence_, "error");
}

void LuaServerChannel::SetDataCallback(OnData callback) {
	dataCallback_ = callback;
}

void LuaServerChannel::SetErrorCallback(OnError callback) {
	errorCallback_ = callback
}

void LuaServerChannel::SetDataReference(int reference) {
	dataRefrence_ = reference;
}
	
int LuaServerChannel::GetDataReference() {
	return dataRefrence_;
}

void LuaServerChannel::SetErrorReference(int reference) {
	errorCallback_ = reference;
}

int LuaServerChannel::GetErrorReference() {
	return errorCallback_;
}

int LuaServerChannel::GetReference() {
	return reference_;
}