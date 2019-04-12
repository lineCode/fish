
#ifndef MESSAGE_REGISTER_H
#define MESSAGE_REGISTER_H
#include "logger/Logger.h"
#include "util/format.h"
#include "network/Channel.h"
#include "util/StreamReader.h"
#include "agent/AgentApp.h"

#define CMD_AUTH (1)
#define CMD_MAX (1024)

typedef void(*HandlerFunc)( Network::Channel* channel, StreamReader& reader );

static HandlerFunc HandlerTable[CMD_MAX];

void DefaultHandler(Network::Channel* channel, StreamReader& reader);

int DispatchHandler(int cmd, Network::Channel* channel, StreamReader& reader);

struct InitializeHandlers {
	InitializeHandlers();
};

struct RegisterHandler {
	RegisterHandler(int cmd, HandlerFunc func);
};

#define REGISTER_HANDLER(cmd) \
	static void handler##cmd(Network::Channel* channel, StreamReader& reader); \
	static RegisterHandler register##cmd(cmd, handler##cmd); \
	static void handler##cmd(Network::Channel* channel, StreamReader& reader)




#define REGISTER_AGENT_HANDLER(cmd, method) \
	static void agent_handler_##cmd(Network::Channel* channel, StreamReader& reader) {\
		AGENT_APP->method(channel, reader); \
	}\
	static RegisterHandler agent_handler_register_##cmd(cmd, agent_handler_##cmd); \
	

#endif



