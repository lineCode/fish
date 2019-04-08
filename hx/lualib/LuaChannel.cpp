#include "LuaChannel.h"
#include "logger/Logger.h"
#include "network/FastReader.h"
#include "network/TcpReader.h"
#include "network/TcpWriter.h"

#define MAX_MESSAGE_SIZE (16*1024*1024)

LuaChannel::LuaChannel(Network::EventPoller* poller,int fd, LuaFish* lua, uint32_t header) : Network::Channel(poller, fd) {
	lua_ = lua;
	header_ = header;
	need_ = 0;
	reference_ = LUA_NOREF;
	dataReference_ = LUA_NOREF;
	closeReference_ = LUA_NOREF;
	errorReference_ = LUA_NOREF;

	SetReader(new Network::FastReader(1024, 1024 * 1024));
	SetWriter(new Network::TcpWriter());
}

LuaChannel::~LuaChannel() {

}

void LuaChannel::UnRefAll() {
	lua_State* L = lua_->GetScript().state();
	luaL_unref(L, LUA_REGISTRYINDEX, reference_);
	luaL_unref(L, LUA_REGISTRYINDEX, dataReference_);
	luaL_unref(L, LUA_REGISTRYINDEX, closeReference_);
	luaL_unref(L, LUA_REGISTRYINDEX, errorReference_);
}

void LuaChannel::HandleRead() {
	lua_State* L = lua_->GetScript().state();

	if (header_ == 0) {
		lua_rawgeti(L, LUA_REGISTRYINDEX, dataReference_);
		lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);
		if (LUA_OK != lua_pcall(L, 1, 0, 0)) {
			LOG_ERROR(fmt::format("HandleRead error:{}", lua_tostring(L, -1)));
		}
		return;
	} 

	while(IsAlive()) {
		if (need_ == 0) {
			if ( reader_->GetTotal() < (int)header_ ) {
				break;
			}
			if (header_ == 2) {
				uint8_t header[2];
				reader_->ReadData((char*)header, 2);
				need_ = header[0] | header[1] << 8;
			} else if (header_ == 4) {
				uint8_t header[4];
				reader_->ReadData((char*)header, 4);
				need_ = header[0] | header[1] << 8 | header[2] << 16 | header[3] << 24;
			}
			need_ -= header_;
			if (need_ > MAX_MESSAGE_SIZE) {
				LOG_ERROR(fmt::format("HandleRead error:message size more than {}mb", MAX_MESSAGE_SIZE/(1024*1024)));
				Close();
				break;
			}
		} else {
			if (reader_->GetTotal() < (int)need_) {
				break;
			}

			char* data = reader_->ReadData(need_);
			
			lua_rawgeti(L, LUA_REGISTRYINDEX, dataReference_);
			lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);
			lua_pushlightuserdata(L, data);
			lua_pushinteger(L, need_);

			if (LUA_OK != lua_pcall(L, 3, 0, 0)) {
				LOG_ERROR(fmt::format("HandleRead error:{}", lua_tostring(L, -1)));
			}

			need_ = 0;
		}
	}
}

void LuaChannel::HandleClose() {
	lua_State* L = lua_->GetScript().state();
	lua_rawgeti(L, LUA_REGISTRYINDEX, closeReference_);
	lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);
	if (LUA_OK != lua_pcall(L, 1, 0, 0)) {
		LOG_ERROR(fmt::format("HandleClose error:{}", lua_tostring(L, -1)));
	}
	UnRefAll();
}

void LuaChannel::HandleError() {
	lua_State* L = lua_->GetScript().state();
	lua_rawgeti(L, LUA_REGISTRYINDEX, errorReference_);
	lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);

	if (LUA_OK != lua_pcall(L, 1, 0, 0)) {
		LOG_ERROR(fmt::format("HandleError error:{}", lua_tostring(L, -1)));
	}
	UnRefAll();
}

void LuaChannel::SetReference(int reference) {
	reference_ = reference;
}

int LuaChannel::GetReference() {
	return reference_;
}

void LuaChannel::SetDataReference(int reference) {
	dataReference_ = reference;
}

int LuaChannel::GetDataReference() {
	return dataReference_;
}

void LuaChannel::SetCloseReference(int reference) {
	closeReference_ = reference;
}

int LuaChannel::GetCloseReference() {
	return closeReference_;
}

void LuaChannel::SetErrorReference(int reference) {
	errorReference_ = reference;
}

int LuaChannel::GetErrorReference() {
	return errorReference_;
}

int LuaChannel::LuaRead(lua_State* L) {
	LuaChannel* channel = (LuaChannel*)lua_touserdata(L, 1);
	int size = luaL_optinteger(L, 2, 0);

	int total = channel->reader_->GetTotal();
	if (total == 0) {
		return 0;
	}

	if (size == 0 || size > total) {
		size = total;
	}

	char* data = (char*)malloc(size);
	channel->reader_->ReadData(data, size);
	lua_pushlstring(L, data, size);
	free(data);

	return 1;
}

int LuaChannel::LuaWrite(lua_State* L) {
	LuaChannel* channel = (LuaChannel*)lua_touserdata(L, 1);
	int header = luaL_optinteger(L, 2, 0);
	int vt = lua_type(L, 3);

	char* data = NULL;
	size_t size;
	switch(vt) {
		case LUA_TSTRING: {
			data = (char*) lua_tolstring(L, 3, &size);
			break;
		}
		case LUA_TLIGHTUSERDATA: {
			data = (char*)lua_touserdata(L, 3);
			size = luaL_checkinteger(L, 4);
			break;
		}
		default: {
			luaL_error(L, "channel:%p write error:unknow lua type:%s", channel, lua_typename(L,vt));
		}
	}

	if (size <= 0) {
		luaL_error(L, "channel:%p write error:empty content", channel);
	}

	char* content = NULL;
	if (header == 0) {
		content = data;
		if (vt == LUA_TSTRING) {
			content = (char*)malloc(size);
			memcpy(content, data, size);
		}
	} else if (header == 2) {
		uint16_t length = size + header;
		content = (char*)malloc(length);
		memcpy(content, &length, 2);
		memcpy(content+2, data, size);
		if (vt == LUA_TLIGHTUSERDATA) {
			free(data);
		}
		size = length;
	} else if (header == 4) {
		uint32_t length = size + header;
		content = (char*)malloc(length);
		memcpy(content, &length, 4);
		memcpy(content+4, data, size);
		if (vt == LUA_TLIGHTUSERDATA) {
			free(data);
		}
		size = length;
	} else {
		luaL_error(L, "channel:%p write error:error header size:%d", channel, header);
	}

	int status = channel->Write(content, (int)size, NULL);

	lua_pushboolean(L, status == 0);
	return 1;
}

int LuaChannel::LuaClose(lua_State* L) {
	LuaChannel* channel = (LuaChannel*)lua_touserdata(L, 1);
	bool rightnow = luaL_optinteger(L, 2, 1);
	channel->Close(rightnow);
	return 0;
}

int LuaChannel::LuaRelease(lua_State* L) {
	LuaChannel* channel = (LuaChannel*)lua_touserdata(L, 1);
	channel->~LuaChannel();
	return 0;
}
