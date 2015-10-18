
#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H
#include "Logger.h"
#include "util/format.h"
#include "network/Session.h"
#include "util/MessageReader.h"

#define CMD_MAX (1024)

typedef void (*HandlerFunc)(Network::Session* session,MessageHelper::MessageReader& reader);

static HandlerFunc HandlerTable[CMD_MAX];

static void DefaultHandler(Network::Session* session,MessageHelper::MessageReader& reader)
{
	LOG_ERROR(fmt::format("error message handler"));
}

int DispatchHandler(int cmd,Network::Session* session,MessageHelper::MessageReader& reader);

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

#define DEFINE_HANDLER(CMD) \
	static void handler##CMD(Network::Session* session,MessageHelper::MessageReader& reader); \
	static RegisterHandler register##CMD(CMD,handler##CMD); \
	static void handler##CMD(Network::Session* session,MessageHelper::MessageReader& reader)
#endif
