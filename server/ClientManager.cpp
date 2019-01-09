#include "ClientManager.h"
#include "ClientChannel.h"
#include "network/Network.h"
#include "FishApp.h"
#include <stdlib.h>
#include <string.h>

#define SERVER_ID(vid) 			(vid & 0xff)
#define CLIENT_ID(vid) 			(vid >> 8)
#define MAKE_VID(id,serverId)	(id << 8 | serverId)

template <>
ClientManager * Singleton<ClientManager>::singleton_ = 0;

ClientManager::ClientManager(uint32_t maxClient, uint8_t serverId) {
	serverId_ = serverId;
	maxClient_ = maxClient;
	allocStep_ = 0;
	size_ = 0;

	clientMgr_ = (ClientChannel**)malloc(sizeof( *clientMgr_ ) * maxClient);
	memset(clientMgr_, 0, sizeof( *clientMgr_ ) * maxClient);

	using namespace std::placeholders;

	acceptor_ = new Network::Acceptor(APP->Poller());
	acceptor_->SetCallback(std::bind(&ClientManager::OnClientAccept, this, _1, _2));

	check_.set(APP->Poller()->GetLoop());
	check_.set<ClientManager, &ClientManager::OnCheck>(this);
	check_.start();

	maxFreq_ = 100;
	maxAlive_ = 60 * 3;
	warnFlow_ = 1024 * 16;
}


ClientManager::~ClientManager() {
	free(clientMgr_);
	delete acceptor_;
	check_.stop();
}

void ClientManager::OnClientAccept(int fd, Network::Addr& addr) {
	if (size_ >= maxClient_) {
		Network::SocketClose(fd);
		return;
	}

	ClientChannel* channel = new ClientChannel(APP->Poller(), fd, AllocVid());
	BindClient(channel->GetVid(), channel);
	channel->EnableRead();
}

void ClientManager::MarkClientDead(ClientChannel* channel) {
	deadClients_.push_back(channel);
}

void ClientManager::OnCheck() {
	for(size_t i = 0; i < deadClients_.size();i++) {
		ClientChannel* channel = deadClients_[i];
		delete channel;
	}
	deadClients_.clear();
}

int ClientManager::AllocVid() {
	if ( size_ >= maxClient_ ) {
		return -1;
	}

	for ( ;; ) {
		int id = (allocStep_++) % maxClient_;
		if ( !clientMgr_[id] ) {
			return MAKE_VID(id, serverId_);
		}
	}
	assert(0);
	return -1;
}

ClientChannel* ClientManager::GetClient(int vid) {
	int serverId = SERVER_ID(vid);
	if ( serverId != serverId_ ) {
		return NULL;
	}
	int id = CLIENT_ID(vid);
	ClientChannel* channel = clientMgr_[id];
	if ( !channel ) {
		return NULL;
	}
	return channel;
}

void ClientManager::BindClient(int vid, ClientChannel* channel) {
	int serverId = SERVER_ID(vid);
	if ( serverId != serverId_) {
		return;
	}
	int id = CLIENT_ID(vid);
	assert(clientMgr_[id] == NULL);
	clientMgr_[id] = channel;
}

void ClientManager::DeleteClient(int vid) {
	int serverId = SERVER_ID(vid);
	if ( serverId != serverId_) {
		return;
	}
	int id = CLIENT_ID(vid);
	assert(clientMgr_[id] != NULL);
	clientMgr_[id] = NULL;
}

int ClientManager::SendClient(int vid, char* data, size_t size) {
	ClientChannel* channel = GetClient(vid);
	if ( !channel ) {
		return -1;
	}
	return channel->Write(data, size);
}

int ClientManager::BroadClient(std::vector<int>& vids, char* data, size_t size) {
	for ( size_t i = 0; i < vids.size();i++ ) {
		int vid = vids[i];
		char* message = (char*)malloc(size);
		memcpy(message, data, size);
		SendClient(vid, message, size);
	}
	free(data);
	return 0;
}

int ClientManager::CloseClient(int vid) {
	ClientChannel* channel = GetClient(vid);
	if ( !channel ) {
		return -1;
	}
	if (!channel->IsAlive()) {
		return -1;
	}
	channel->Close(true);
	return 0;
}

int ClientManager::Start(Network::Addr& addr) {
	return acceptor_->Listen(addr);
}

int ClientManager::Stop() {
	return acceptor_->Close();
}

void ClientManager::SetMaxFreq(uint32_t freq) {
	maxFreq_ = freq;
}

uint32_t ClientManager::GetMaxFreq() {
	return maxFreq_;
}

void ClientManager::SetMaxAlive(uint32_t alive) {
	maxAlive_ = alive;
}

uint32_t ClientManager::GetMaxAlive() {
	return maxAlive_;
}

void ClientManager::SetWarnFlow(uint32_t flow) {
	warnFlow_ = flow;
}

uint32_t ClientManager::GetWarnFlow() {
	return warnFlow_;
}