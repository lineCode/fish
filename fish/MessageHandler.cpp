#include "MessageHandler.h"
#include "ClientSession.h"
#include "FishMongo.h"
#include <iostream>


InitializeHandlers gInitializeHandlers;

int DispatchHandler(int cmd,Network::Session* session,MessageHelper::MessageReader& reader)
{
	HandlerTable[cmd](session,reader);

	return 0;
}

REGISTER_HANDLER(CMD_AUTH)
{
	std::string key[6];
	bool valBool;
	std::string valStr;
	int valInt0,valInt1;
	float valFloat;
	double valDouble;

	reader >> key[0] >> valBool >> key[1] >> valStr >> key[2] >> valInt0 >> key[3] >> valFloat >> key[4] >> valDouble >> key[5] >> valInt1;

	std::cout << fmt::format("{} {} {} {} {} {} {} {} {} {} {} {}",key[0],valBool,key[1],valStr,key[2],valInt0,key[3],valDouble,key[4],valDouble,key[5],valInt1) << std::endl;

	FishMongo* mongo = (FishMongo*)((ClientSession*)session)->GetApp()->Mongo();
	mongo->LoadRole(1);
}