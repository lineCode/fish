#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H
#include <stdint.h>
#include "util/Singleton.h"
#include "time/Timer.h"
#include "network/Acceptor.h"
#include <vector>

class ClientChannel;

class ClientManager : public Singleton<ClientManager> {
public:
	ClientManager(uint32_t maxClient, uint8_t serverId);
	~ClientManager();

	void OnClientAccept(int fd, Network::Addr& addr);

	void MarkClientDead(ClientChannel* channel);

	void OnCheck();

	int AllocVfd();

	ClientChannel* GetClient(int id);

	void BindClient(int id, ClientChannel* channel);

	void DeleteClient(int id);

	int SendClient(int id, char* data, size_t size);

	int BroadClient(std::vector<int>& ids, char* data, size_t size);

	int CloseClient(int id);

	int Start(Network::Addr& addr);

	int Stop();

	void SetMaxFreq(uint32_t freq);

	uint32_t GetMaxFreq();

	void SetMaxAlive(uint32_t alive);

	uint32_t GetMaxAlive();

	void SetWarnFlow(uint32_t flow);	

	uint32_t GetWarnFlow();

private:
	uint8_t serverId_;
	uint32_t maxClient_;
	uint32_t allocStep_;
	uint32_t size_;
	ClientChannel** clientMgr_;

	Network::Acceptor* acceptor_;
	ev::check check_;

	std::vector<ClientChannel*> deadClients_;

	uint32_t maxFreq_;
	uint32_t maxAlive_;
	uint32_t warnFlow_;
};

#define CLIENT_MGR ClientManager::GetSingleton()

#endif