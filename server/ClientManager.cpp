#include "ClientManager.h"
#include "ClientChannel.h"
#include "network/Network.h"
#include "FishApp.h"
#include "logger/Logger.h"
#include <stdlib.h>
#include <string.h>


#define SERVER_ID(vid) 			(vid & 0xff)
#define CLIENT_ID(vid) 			(vid >> 8)
#define MAKE_VID(id,serverId)	(id << 8 | serverId)

using namespace std::placeholders;

template <>
ClientManager * Singleton<ClientManager>::singleton_ = 0;

ClientManager::ClientManager(uint32_t maxClient, uint8_t serverId) {
	serverId_ = serverId;
	maxClient_ = maxClient;
	allocStep_ = 0;
	size_ = 0;

	clientSlots_ = (ClientChannel**)malloc(sizeof(*clientSlots_) * maxClient);
	memset(clientSlots_, 0, sizeof(*clientSlots_) * maxClient);

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
	free(clientSlots_);
	delete acceptor_;
	check_.stop();
}

void ClientManager::OnClientAccept(int fd, Network::Addr& addr) {
	if (size_ >= maxClient_) {
		Network::SocketClose(fd);
		LOG_INFO(fmt::format("the number of client has limited:{}", size_));
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
		if ( !clientSlots_[id] ) {
			return MAKE_VID(id, serverId_);
		}
	}
	assert(0);
	return -1;
}

ClientChannel* ClientManager::GetClient(int vid) {
	int serverId = SERVER_ID(vid);
	if ( serverId != serverId_ ) {
		LOG_ERROR(fmt::format("invalid vid:{}, serverId:{} {}", vid, serverId, serverId_));
		return NULL;
	}
	
	int id = CLIENT_ID(vid);
	if (id < 0 || id >= maxClient_) {
		LOG_ERROR(fmt::format("invalid vid:{}, id:{}", vid, id));
		return NULL;
	}

	ClientChannel* channel = clientSlots_[id];
	if ( !channel ) {
		return NULL;
	}
	return channel;
}

void ClientManager::BindClient(int vid, ClientChannel* channel) {
	int serverId = SERVER_ID(vid);
	if ( serverId != serverId_) {
		LOG_ERROR(fmt::format("invalid vid:{}, serverId:{} {}", vid, serverId, serverId_));
		return;
	}
	int id = CLIENT_ID(vid);
	if (id < 0 || id >= maxClient_) {
		LOG_ERROR(fmt::format("invalid vid:{}, id:{}", vid, id));
		return;
	}
	assert(clientSlots_[id] == NULL);
	clientSlots_[id] = channel;
}

void ClientManager::DeleteClient(int vid) {
	int serverId = SERVER_ID(vid);
	if ( serverId != serverId_) {
		LOG_ERROR(fmt::format("invalid vid:{}, serverId:{} {}", vid, serverId, serverId_));
		return;
	}
	int id = CLIENT_ID(vid);
	if (id < 0 || id >= maxClient_) {
		LOG_ERROR(fmt::format("invalid vid:{}, id:{}", vid, id));
		return;
	}
	assert(clientSlots_[id] != NULL);
	clientSlots_[id] = NULL;
}

int ClientManager::SendClient(int vid, char* data, size_t size) {
	ClientChannel* channel = GetClient(vid);
	if ( !channel ) {
		LOG_ERROR(fmt::format("no such client:{}", vid));
		return -1;
	}
	return channel->Write(data, size);
}

int ClientManager::BroadcastClient(std::vector<int>& vids, char* data, size_t size) {
	if ( vids.size() == 1 ) {
		return SendClient(vids[0], data, size);
	}

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
		LOG_ERROR(fmt::format("no such client:{}", vid));
		return -1;
	}
	if (!channel->IsAlive()) {
		LOG_ERROR(fmt::format("client:{} no alive", vid));
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


int ClientManager::Register(lua_State* L) {
	luaL_checkversion(L);

	luaL_Reg methods[] = {
		{ "Stop", ClientManager::LStop },
		{ "Send", ClientManager::LSendClient },
		{ "Broadcast", ClientManager::LBroadcastClient },
		{ "Close", ClientManager::LCloseClient },
		{ NULL, NULL },
	};

	luaL_newlibtable(L, methods);
	luaL_setfuncs(L, methods, 0);

	return 1;
}

int ClientManager::LStop(lua_State* L) {
	CLIENT_MGR->Stop();
	return 0;
}

int ClientManager::LSendClient(lua_State* L) {
	int vid = luaL_checkinteger(L, 1);
	int vt = lua_type(L, 2);

	char* data = NULL;
	size_t size;
	switch(vt) {
		case LUA_TSTRING: {
			const char* tmp = lua_tolstring(L, 2, &size);
			data = (char*)malloc(size);
			memcpy(data, tmp, size);
			break;
		}
		case LUA_TLIGHTUSERDATA: {
			data = (char*)lua_touserdata(L, 2);
			size = luaL_checkinteger(L, 3);
		}
		default: {
			luaL_error(L, "client manager write error:unknow lua type:%s", lua_typename(L,vt));
		}
	}

	if (size <= 0) {
		return 0;
	}

	CLIENT_MGR->SendClient(vid, data, size);
	return 0;
}

int ClientManager::LBroadcastClient(lua_State* L) {
	return 0;
}

int ClientManager::LCloseClient(lua_State* L) {
	int vid = luaL_checkinteger(L, 1);
	CLIENT_MGR->CloseClient(vid);
	return 0;
}
