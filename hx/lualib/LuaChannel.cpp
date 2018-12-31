#include "LuaChannel.h"

LuaChannel::LuaChannel(Network::EventPoller* poller,int fd, LuaFish* lua, uint32_t header) : Network::Channel(poller, fd) {
	lua_ = lua;
	header_ = header;
	need_ = 0;
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
	
	if (header_ == 0) {
		if (LUA_OK != lua_pcall(L, 1, 0, 0)) {
			LOG_ERROR(fmt::format("HandleRead error:{}", lua_tostring(L, -1)));
		}
	} else {
		while(IsAlive()) {
			if (need_ == 0) {
				if (reader_->total_ < header_) {
					break;
				}
				if (header_ == 2) {
					uint8_t header[2];
					reader_->ReadData(header, 2);
					need_ = header[0] | header[1] << 8;
				} else if (header_ == 4) {
					uint8_t header[4];
					reader_->ReadData(header, 4);
					need_ = header[0] | header[1] << 8 | header[2] << 16 | header[3] << 24;
				}
			} else {
				if (reader_->total_ < need_) {
					break;
				}

				char* data = malloc(need_);
				reader_->ReadData(data, need_);
				
				lua_pushlightuserdata(L, data);
				lua_pushinteger(L, need_);

				if (LUA_OK != lua_pcall(L, 3, 0, 0)) {
					LOG_ERROR(fmt::format("HandleRead error:{}", lua_tostring(L, -1)));
				}

				need_ = 0;
				free(data);
			}
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
	LuaChannel* channel = (LuaChannel*)lua_touserdata(L, 1);
	int size = luaL_optinteger(L, 2, 0);

	int total = reader_->total_;
	if (total == 0) {
		return 0;
	}

	if (size == 0 || size > total) {
		size = total;
	}

	char* data = mallo(size);
	reader_->ReadData(data, size);
	lua_pushlstring(L, data, size);
	free(data);

	return 1;
}

int LuaChannel::Write(lua_State* L) {

}

int LuaChannel::Close(lua_State* L) {


}

int LuaChannel::Release(lua_State* L) {

}