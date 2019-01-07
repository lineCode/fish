#include "LuaHttpChannel.h"
#include "logger/Logger.h"

LuaHttpChannel::LuaHttpChannel(Network::EventPoller* poller, int fd, LuaFish* lua) : Network::HttpChannel(poller, fd) {
	lua_ = lua;
	reference_ = 0;
}

LuaHttpChannel::~LuaHttpChannel() {
}


void LuaHttpChannel::HandleClose() {
	lua_State* L = lua_->GetScript().state();
	luaL_unref(L, LUA_REGISTRYINDEX, reference_);
}

void LuaHttpChannel::HandleError() {
	lua_State* L = lua_->GetScript().state();
	luaL_unref(L, LUA_REGISTRYINDEX, reference_);
}

void LuaHttpChannel::SetReference(int reference) {
	reference_ = reference;
}

int LuaHttpChannel::GetReference() {
	return reference_;
}

int LuaHttpChannel::LGetUrl(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	std::string url = channel->GetUrl();
	lua_pushlstring(L, url.c_str(), url.length());
	return 1;
}

int LuaHttpChannel::LGetHeader(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	std::map<std::string, std::string>& headers = channel->GetHeader();
	std::map<std::string, std::string>::iterator iter = headers.begin();

	lua_newtable(L);
	for(;iter != headers.end();iter++) {
		const std::string& k = iter->first;
		const std::string& v = iter->second;

		lua_pushlstring(L, k.c_str(), k.length());
		lua_pushlstring(L, v.c_str(), v.length());

		lua_settable(L, -3);
	}
	return 1;
}

int LuaHttpChannel::LGetContent(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	std::string content = channel->GetContent();
	lua_pushlstring(L, content.c_str(), content.length());
	return 1;
}

int LuaHttpChannel::LSetReplyHeader(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	return 0;
}

int LuaHttpChannel::LReply(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	int code = luaL_checkinteger(L, 2);
	size_t size;
	const char* content = luaL_checklstring(L, 3, &size);
	std::string body(content);
	channel->Reply(code, body);
	return 0;
}

int LuaHttpChannel::LClose(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	bool rightnow = luaL_optinteger(L, 2, 1);
	channel->Close(rightnow);
	return 0;
}

int LuaHttpChannel::LRelease(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	channel->~LuaHttpChannel();
	return 0;
}
