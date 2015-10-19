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

DEFINE_HANDLER(CMD_AUTH)
{
	std::string str0,str1,str2,str3,str4,str5;
	bool valBool;
	std::string valStr;
	int valInt0,valInt1;
	float valFloat;
	double valDouble;

	reader >> str0 >> valBool >> str1 >> valStr >> str2 >> valInt0 >> str3 >> valFloat >> str4 >> valDouble >> str5 >> valInt1;

	std::cout << fmt::format("{} {} {} {} {} {} {} {} {} {} {} {}",str0,valBool,str1,valStr,str2,valInt0,str3,valDouble,str4,valDouble,str5,valInt1) << std::endl;

	FishMongo* mongo = (FishMongo*)((ClientSession*)session)->GetApp()->Mongo();
	mongo->LoadRoles();
}