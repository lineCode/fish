#include "LuaHttpChannel.h"
#include "logger/Logger.h"

LuaHttpChannel::LuaHttpChannel(Network::EventPoller* poller, int fd, LuaFish* lua) : Network::HttpChannel(poller, fd) {
	lua_ = lua;
	reference_ = LUA_NOREF;
	callback_ = LUA_NOREF;
}

LuaHttpChannel::~LuaHttpChannel() {
}

void LuaHttpChannel::HandleRead() {
	Network::HttpChannel::HandleRead();

	if ( !GetComplete() ) {
		return;
	}
	lua_State* L = lua_->GetScript().state();

	lua_rawgeti(L, LUA_REGISTRYINDEX, callback_);
	lua_rawgeti(L, LUA_REGISTRYINDEX, reference_);
	if ( LUA_OK != lua_pcall(L, 1, 0, 0) ) {
		LOG_ERROR(fmt::format("LuaHttpChannel::HandleRead error:{}", lua_tostring(L, -1)));
	}
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

void LuaHttpChannel::SetCallback(int callback) {
	callback_ = callback;
}

int LuaHttpChannel::LuaGetUrl(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	std::string url = channel->GetUrl();
	lua_pushlstring(L, url.c_str(), url.length());
	return 1;
}

int LuaHttpChannel::LuaGetHeader(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	HttpChannel::HeadersMap& headers = channel->GetHeader();
	HttpChannel::HeadersMap::iterator iter = headers.begin();

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

int LuaHttpChannel::LuaGetContent(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	std::string content = channel->GetContent();
	lua_pushlstring(L, content.c_str(), content.length());
	return 1;
}

int LuaHttpChannel::LuaSetReplyHeader(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	const char* field = luaL_checkstring(L, 2);
	const char* value = luaL_checkstring(L, 3);
	channel->SetReplyHeader(field, value);
	return 0;
}

int LuaHttpChannel::LuaReply(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	int code = luaL_checkinteger(L, 2);
	size_t size;
	const char* content = luaL_checklstring(L, 3, &size);
	channel->Reply(code, (char*)content, size);
	return 0;
}

int LuaHttpChannel::LuaClose(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	bool rightnow = luaL_optinteger(L, 2, 1);
	channel->Close(rightnow);
	return 0;
}

int LuaHttpChannel::LuaRelease(lua_State* L) {
	LuaHttpChannel* channel = (LuaHttpChannel*)lua_touserdata(L, 1);
	channel->~LuaHttpChannel();
	return 0;
}
