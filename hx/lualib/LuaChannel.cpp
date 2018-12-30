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
	lua_State* L = lua->GetScript()->state();

	lua_rawgeti(L, LUA_REGISTRYINDEX, dataReference_);
	lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);
	
	switch(header_) {
		case 0: {

		}

		case 2: {

		}

		case 4: {

		}
	}
}

void LuaChannel::HandleClose() {
	lua_State* L = lua->GetScript()->state();
	lua_rawgeti(L, LUA_REGISTRYINDEX, closeReference_);
	lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);

	if (LUA_OK != lua_pcall(L, 1, 0, 0)) {
		LOG_ERROR(fmt::format("HandleClose error:{}", lua_tostring(L, -1)));
	}
}

void LuaChannel::HandleError() {
	lua_State* L = lua->GetScript()->state();
	lua_rawgeti(L, LUA_REGISTRYINDEX, errorReference_);
	lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);

	if (LUA_OK != lua_pcall(L, 1, 0, 0)) {
		LOG_ERROR(fmt::format("HandleError error:{}", lua_tostring(L, -1)));
	}
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

int LuaChannel::Release(lua_State* L) {

}