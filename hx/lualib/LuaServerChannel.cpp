#include "LuaServerChannel.h"

LuaServerChannel::LuaServerChannel(Network::EventPoller* poller, int fd, int reference) : Channel(poller, fd) {
	reference_ = reference;
}

LuaServerChannel::~LuaServerChannel() {

}

void LuaServerChannel::HandleRead() {
	dataCallback_(reference_, dataReference_, NULL, 0);
}

void LuaServerChannel::HandleClose() {
	errorCallback_(reference_, errorReference_, "closed");
}

void LuaServerChannel::HandleError() {
	errorCallback_(reference_, errorReference_, "error");
}

void LuaServerChannel::SetDataCallback(OnData callback) {
	dataCallback_ = callback;
}

void LuaServerChannel::SetErrorCallback(OnError callback) {
	errorCallback_ = callback;
}

void LuaServerChannel::SetDataReference(int reference) {
	dataReference_ = reference;
}
	
int LuaServerChannel::GetDataReference() {
	return dataReference_;
}

void LuaServerChannel::SetErrorReference(int reference) {
	errorReference_ = reference;
}

int LuaServerChannel::GetErrorReference() {
	return errorReference_;
}

int LuaServerChannel::GetReference() {
	return reference_;
}
