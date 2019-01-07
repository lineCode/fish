
#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H
#include "logger/Logger.h"
#include "util/format.h"
#include "network/Channel.h"
#include "util/MessageReader.h"

#define CMD_AUTH (1)
#define CMD_MAX (1024)

typedef void(*HandlerFunc)( Network::Channel* channel, MessageHelper::MessageReader& reader );

static HandlerFunc HandlerTable[CMD_MAX];

void DefaultHandler(Network::Channel* channel, MessageHelper::MessageReader& reader);

int DispatchHandler(int cmd, Network::Channel* channel, MessageHelper::MessageReader& reader);

struct InitializeHandlers {
	InitializeHandlers();
};

struct RegisterHandler {
	RegisterHandler(int cmd, HandlerFunc func);
};

#define REGISTER_HANDLER(cmd,channel,reader) \
	static void handler##cmd(Network::Channel* channel, MessageHelper::MessageReader& reader); \
	static RegisterHandler register##cmd(cmd,handler##cmd); \
	static void handler##cmd(Network::Channel* channel, MessageHelper::MessageReader& reader)
#endif
