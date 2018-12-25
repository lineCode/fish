#include "MessageHandler.h"
#include "ClientSession.h"

#include <iostream>


InitializeHandlers gInitializeHandlers;

int DispatchHandler(int cmd,Network::Session* session,MessageHelper::MessageReader& reader)
{
	HandlerTable[cmd](session,reader);

	return 0;
}

