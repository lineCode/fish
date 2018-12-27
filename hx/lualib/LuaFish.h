#ifndef LUAFISH_H
#define LUAFISH_H
#include "util/LuaAllocator.h"
#include "LuaTimer.h"
#include "network/Address.h"
#include "oolua.h"
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

class ServerApp;

class LuaFish
{
public:
	LuaFish(void);
	~LuaFish(void);

	lua_State* LuaState();

	OOLUA::Script& GetScript();

	int Init(ServerApp* app);

	int LoadFile(std::string& file);

	int DoFile(const char* file);

	int DoFile(std::string& file);

	int CallFunc(const char* module, const char* method);

	int CallFunc(std::string& module, std::string& method);

	void LuaPath(const char* path);

	void Require(const char* module,int (*func)(lua_State*));

	uint64_t GenTimerId();

	void OnTimeout(uint64_t timerId);

	void OnAccept(int fd, Network::Addr& addr, void* userdata);

	void OnConnect(int fd, const char* reason, void* userdata);

	void OnData(int object, int callback, char* data, size_t size);

	void OnError(int object, int callback, const char* reason);

	static int Register(lua_State* L);

	static int Log(lua_State* L);

	static int Now(lua_State* L);

	static int Timestamp(lua_State* L);

	static int TimestampToSecond(lua_State* L);

	static int TimerStart(lua_State* L);

	static int TimerCancel(lua_State* L);

	static int AcceptorListen(lua_State* L);

	static int AcceptorClose(lua_State* L);

	static int AcceptorRelease(lua_State* L);

	static int ConnectorConnect(lua_State* L);

	static int ConnectorClose(lua_State* L);

	static int ConnectorRelease(lua_State* L);

	static int CreateBaseChannel(lua_State* L);

	static int CreateServerChannel(lua_State* L);

	static int ServerChannelClose(lua_State* L);

	static int ServerChannelRelease(lua_State* L);

	static int CreateHttpChannel(lua_State* L);

	static int Stop(lua_State* L);
protected:
	OOLUA::Script script_;

	uint64_t timerCount_;
};

#endif
