#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H
#include <stdint.h>
#include "util/Singleton.h"
#include "time/Timer.h"
#include "network/Acceptor.h"
#include "ServerApp.h"
#include "lua.hpp"
#include <vector>

#define DEFAULT_BUFF_SIZE (64 * 1024)



namespace Network {
	class ClientChannel;
	class ClientManager : public Singleton<ClientManager> {
	public:
		ClientManager(ServerApp* app, uint32_t maxClient, uint8_t serverId);

		~ClientManager();

		void OnClientAccept(int fd, Addr& addr);

		void MarkClientDead(ClientChannel* channel);

		void OnCheck();

		int AllocVid();

		ClientChannel* GetClient(int vid);

		void BindClient(int vid, ClientChannel* channel);

		void DeleteClient(int vid);

		int SendClient(int vid, char* data, size_t size);

		int BroadcastClient(std::vector<int>& vids, char* data, size_t size);

		int CloseClient(int vid);

		int Start(Addr& addr);

		int Stop();

		void SetMaxFreq(uint32_t freq);

		uint32_t GetMaxFreq();

		void SetMaxAlive(uint32_t alive);

		uint32_t GetMaxAlive();

		void SetWarnFlow(uint32_t flow);

		uint32_t GetWarnFlow();

		ServerApp* GetApp();

		uint8_t* AllocBuffer(size_t size);

		void FreeBuffer(uint8_t* buffer);

		static int Register(lua_State* L);

		static int LuaStop(lua_State* L);

		static int LuaSendClient(lua_State* L);

		static int LuaBroadcastClient(lua_State* L);

		static int LuaCloseClient(lua_State* L);

	private:
		ServerApp* app_;

		uint8_t serverId_;

		uint32_t maxClient_;
		uint32_t allocStep_;
		uint32_t size_;

		ClientChannel** clientSlots_;

		Acceptor* acceptor_;
		ev::check check_;

		std::vector<ClientChannel*> deadClients_;

		uint32_t maxFreq_;
		uint32_t maxAlive_;
		uint32_t warnFlow_;

		uint8_t buffer_[DEFAULT_BUFF_SIZE];
	};
}

#define CLIENT_MGR Network::ClientManager::GetSingleton()

#endif