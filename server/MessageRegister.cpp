#include "MessageRegister.h"
#include <iostream>
#include <assert.h>


InitializeHandlers gInitializeHandlers;

void DefaultHandler(Network::Channel* channel, StreamReader& reader) {
	LOG_ERROR(fmt::format("error message handler"));
}

int DispatchHandler(int cmd, Network::Channel* channel, StreamReader& reader) {
	HandlerTable[cmd](channel, reader);
	return 0;
}

InitializeHandlers::InitializeHandlers() {
	for ( int i = 0; i < CMD_MAX; i++ ) {
		HandlerTable[i] = DefaultHandler;
	}
}

RegisterHandler::RegisterHandler(int cmd, HandlerFunc func) {
	if (cmd >= CMD_MAX) {
		assert(false);
	}
	HandlerTable[cmd] = func;
}



