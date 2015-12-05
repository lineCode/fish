#ifndef MONGOQUERY_H
#define MONGOQUERY_H

class MongoQuery
{
public:
	MongoQuery(void);
	virtual ~MongoQuery(void);

	virtual void Reply(char* data,int size) = 0;
};

#endif