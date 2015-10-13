#ifndef FISHAPP_H
#define FISHAPP_H

#include "ServerApp.h"
#include <vector>
#include <map>
#include <string>

#include "network/Connector.h"
#include "mongo/MongoSession.h"


class FishApp:public ServerApp
{
public:
	FishApp(std::string file);

	~FishApp(void);

	virtual int Init();

	virtual void ConnectMongo(const char* host,int port);

	virtual int Fina();

	virtual int Run();

	
private:
	std::string _file;
	Network::Connector<MongoSession> _mongoConnector;
};

#endif