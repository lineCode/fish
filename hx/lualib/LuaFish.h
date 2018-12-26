#ifndef LUAFISH_H
#define LUAFISH_H
#include "../util/LuaAllocator.h"
#include "LuaTimer.h"
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
	enum SocketEvent
	{
		Data = 1,
		Accept,
		Connect,
		Close
	};
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

	LuaTimer* GetTimer(int timerId);

	void BindTimer(int timerId, LuaTimer* timer);

	void DeleteTimer(int timerId);

	void OnTimeout(LuaTimer* timer, void* userdata);

	static int Register(lua_State* L);

	static int Log(lua_State* L);

	static int Now(lua_State* L);

	static int Timestamp(lua_State* L);

	static int TimestampToSecond(lua_State* L);

	static int StartTimer(lua_State* L);

	static int CancelTimer(lua_State* L);

	static int Stop(lua_State* L);
protected:
	OOLUA::Script script_;

	std::map<int, LuaTimer*> timerMgr_;
};

#endif
