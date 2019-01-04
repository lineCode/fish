#ifndef LOGGERAPP_H
#define LOGGERAPP_H


#include <vector>
#include <map>
#include <string>

#include "ServerApp.h"
#include "network/Acceptor.h"
#include "network/Address.h"

class LoggerApp : public ServerApp
{
public:
	LoggerApp();

	~LoggerApp(void);

	virtual int Init();

	virtual int Fina();

	virtual int Run();

private:
	

};

#endif
