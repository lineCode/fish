#ifndef LUAFISH_H
#define LUAFISH_H

#include "lua.hpp"
#include "oolua.h"

#include "util/ObjectPool.h"
#include "network/Address.h"
#include "time/Timer.h"
#include <unordered_map>

class ServerApp;

class LuaFish
{
public:
	typedef std::unordered_map<uint64_t, Timer*> TimerMap;
	
public:
	LuaFish(void);
	~LuaFish(void);

	OOLUA::Script& GetScript();

	int Init(ServerApp* app);

	int LoadFile(std::string& file);

	int DoFile(const char* file);

	int DoFile(std::string& file);

	void SetPath(const char* path);

	void Require(const char* module,int (*func)(lua_State*));

	void CreateMetaTable(const char* name, const luaL_Reg meta[], lua_CFunction gc);

	uint64_t AllocTimer(Timer*& timer);

	int DeleteTimer(uint64_t timerId);
	
	Timer* GetTimer(uint64_t timerId);

	void OnTimeout(Timer* timer, uint64_t timerId, void* userdata);

	void OnAccept(int fd, Network::Addr& addr, void* userdata);

	void OnConnect(int fd, const char* reason, void* userdata);

	static int Register(lua_State* L);

	static int Stop(lua_State* L);

	static int WriteLog(lua_State* L);

	static int SendLog(lua_State* L);

	static int Now(lua_State* L);

	static int Timestamp(lua_State* L);

	static int Dump(lua_State* L);

	static int TimerStart(lua_State* L);

	static int TimerCancel(lua_State* L);
	
	static int AcceptorListen(lua_State* L);

	static int AcceptorClose(lua_State* L);

	static int AcceptorRelease(lua_State* L);

	static int ConnectorConnect(lua_State* L);

	static int ConnectorClose(lua_State* L);

	static int ConnectorRelease(lua_State* L);

	static int BindChannel(lua_State* L);

	static int BindHttpChannel(lua_State* L);
protected:
	OOLUA::Script script_;
	TimerMap timerMgr_;
	uint64_t timerStep_;
};

#endif
