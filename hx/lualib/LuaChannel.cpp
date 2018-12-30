#include "LuaChannel.h"

LuaChannel::LuaChannel(Network::EventPoller* poller,int fd, LuaFish* lua, uint32_t header) : Network::Channel(poller, fd) {
	lua_ = lua;
	header_ = header;
	reference_ = 0;
	dataRefernce_ = 0;
	closeRefernce_ = 0;
	errorRefernce_ = 0;
}

LuaChannel::~LuaChannel() {

}

void LuaChannel::HandleRead() {

}

void LuaChannel::HandleClose() {

}

void LuaChannel::HandleError() {

}

void LuaChannel::SetRefernce(int reference) {
	reference_ = reference;
}

void LuaChannel::SetDataReference(int reference) {
	dataReference_ = reference;
}

void LuaChannel::SetCloseReference(int reference) {
	closeReference_ = reference;
}

void LuaChannel::SetErrorReference(int reference) {
	errorReference_ = reference;
}

int LuaChannel::Read(lua_State* L) {

}

int LuaChannel::Write(lua_State* L) {

}

int LuaChannel::Close(lua_State* L) {


}

