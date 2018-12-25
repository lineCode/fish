#include "MessageHandler.h"


#include <iostream>


InitializeHandlers gInitializeHandlers;

int DispatchHandler(int cmd,Network::Channel* channel,MessageHelper::MessageReader& reader)
{
	HandlerTable[cmd](channel, reader);

	return 0;
}

