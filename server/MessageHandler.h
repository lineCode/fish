
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

static void DefaultHandler(Network::Channel* channel, MessageHelper::MessageReader& reader)
{
	LOG_ERROR(fmt::format("error message handler"));
}

int DispatchHandler(int cmd, Network::Channel* channel, MessageHelper::MessageReader& reader);

struct InitializeHandlers
{
	InitializeHandlers()
	{
		for (int i = 0;i < CMD_MAX;i++)
		{
			HandlerTable[i] = DefaultHandler;
		}
	}
};

struct RegisterHandler
{
	RegisterHandler(int cmd,HandlerFunc func)
	{
		HandlerTable[cmd] = func;
	}
};

#define REGISTER_HANDLER(cmd,channel,reader) \
	static void handler##cmd(Network::Channel* channel, MessageHelper::MessageReader& reader); \
	static RegisterHandler register##cmd(cmd,handler##cmd); \
	static void handler##cmd(Network::Channel* channel, MessageHelper::MessageReader& reader)
#endif
