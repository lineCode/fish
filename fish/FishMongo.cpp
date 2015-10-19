#include "FishMongo.h"
#include "mongo/MongoCursor.h"
#include <iostream>

FishMongo::FishMongo(Network::EventPoller * poller,int fd):Super(poller,fd)
{
}


FishMongo::~FishMongo(void)
{
}

void FishMongo::LoadRoles()
{
	bson::BSONObj query;
	bson::BSONObj selector;
	this->Query(boost::bind(&FishMongo::LoadRolesReply,this,_1,_2),"u3d.role",query,selector,0,0,10000);
}

void FishMongo::LoadRolesReply(const char* data,int size)
{
	MongoCursor cursor(data,size);
	while (cursor.More())
	{
		char* next = cursor.Next();
		bson::BSONObj bobj(next);
		std::cout << bobj << std::endl;
	}
}