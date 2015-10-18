#include "MessageHandler.h"

InitializeHandlers gInitializeHandlers;

int DispatchHandler(int cmd,Network::Session* session,MessageHelper::MessageReader& reader)
{
	HandlerTable[cmd](session,reader);

	return 0;
}

DEFINE_HANDLER(1)
{

}