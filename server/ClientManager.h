#ifndef CLIENT_MANAGER_H
#define CLIENT_MANAGER_H
#include <stdint.h>
#include "util/Singleton.h"
#include "time/Timer.h"
#include "network/Acceptor.h"
#include <vector>

class ClientChannel;

class ClientManager : public Singleton<ClientManager>
{
public:
	ClientManager(Network::EventPoller* poller, uint32_t maxClient, uint8_t serverId);
	~ClientManager();

	void OnClientAccept(int fd, Network::Addr& addr);

	void MarkClientDead(ClientChannel* channel);

	void OnUpate(Timer* timer, void* userdata);

	void OnCheck();

	int AllocVfd();

	void BindClient(int id, ClientChannel* channel);

	void DeleteClient(int id);

	ClientChannel* GetClient(int id);

	int SendClient(int id, char* data, size_t size);

	int BroadClient(std::vector<int>& ids, char* data, size_t size);

	int CloseClient(int id);

	int Listen(Network::Addr& addr);

private:
	uint8_t serverId_;
	uint32_t maxClient_;
	uint32_t allocStep_;
	uint32_t size_;
	ClientChannel** clientMgr_;

	Timer* timer_;
	Network::Acceptor* acceptor_;
	Network::EventPoller* poller_;

	ev::check check_;

	std::vector<ClientChannel*> deadClients_;
};

#endif