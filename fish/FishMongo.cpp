#include "FishMongo.h"
#include "mongo/MongoCursor.h"
#include "mongo/MongoQuery.h"
#include <iostream>

class LoadRoleQuery : public MongoQuery
{
public:
	LoadRoleQuery(int id)
	{
		_id = id;
	}

	void Reply(char* data,int size)
	{
		MongoCursor cursor(data,size);
		while (cursor.More())
		{
			char* next = cursor.Next();
			bson::BSONObj bobj(next);
			std::cout << bobj << std::endl;
		}
	}

private:
	int _id;
};

FishMongo::FishMongo(Network::EventPoller * poller,int fd):Super(poller,fd)
{
}

FishMongo::~FishMongo(void)
{
}

void FishMongo::LoadRole(int id)
{
	bson::BSONObj selector;

	bson::BSONObjBuilder query;

	this->Query(new LoadRoleQuery(id),"u3d.role",query.obj(),selector,0,0,10000);
}

