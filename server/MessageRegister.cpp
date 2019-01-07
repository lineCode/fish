#include "MessageRegister.h"
#include <iostream>


InitializeHandlers gInitializeHandlers;

void DefaultHandler(Network::Channel* channel, MessageHelper::MessageReader& reader) {
	LOG_ERROR(fmt::format("error message handler"));
}

int DispatchHandler(int cmd, Network::Channel* channel, MessageHelper::MessageReader& reader) {
	HandlerTable[cmd](channel, reader);
	return 0;
}

InitializeHandlers::InitializeHandlers() {
	for ( int i = 0; i < CMD_MAX; i++ ) {
		HandlerTable[i] = DefaultHandler;
	}
}

RegisterHandler::RegisterHandler(int cmd, HandlerFunc func) {
	HandlerTable[cmd] = func;
}



