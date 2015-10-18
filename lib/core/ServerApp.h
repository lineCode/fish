#ifndef SERVERAPP_H
#define SERVERAPP_H

#include "boost/function.hpp"
#include "network/EventPoller.h"
#include "util/MemoryStream.h"
#include "util/MessageWriter.h"
#include "util/MessageReader.h"
#include "util/BufferHelper.h"
#include "util/ObjectPool.h"
#include "network/Session.h"
#include "lualib/LuaFish.h"
#include "time/TimerHandler.h"


class MongoSession;

class ServerApp : public TimeoutHandler
{
public:
	typedef std::map<int,Network::Session*>	SessionMap;

public:
	ServerApp(void);
	virtual ~ServerApp(void);

	virtual int						Init();

	virtual int						Fina();

	virtual int						Run();

	virtual int						HandleTimeout();

	virtual void					SessionEnter(int source,Network::Session* session);

	virtual void					SessionLeave(int source);

	virtual Network::Session*		FindSession(int source);

	virtual LuaFish*				LuaManager();

	virtual Network::EventPoller*	Poller();

	virtual uint64					Now();

	virtual int						Dispatch(int fd,const char* ptr,int size);

	void							Mongo(Network::Session* mongo);

	Network::Session*				Mongo();

	ObjectPool<MemoryStream>&		StreamPool();

	ObjectPool<MessageHelper::MessageWriter>& WriterPool();

protected:
	Network::EventPoller * _poller;
	LuaFish*			   _LuaManager;
	Network::Session*	   _mongo;
	SessionMap			   _sessionMap;
	uint64				   _now;
	ObjectPool<MemoryStream> _streamPool;
	ObjectPool<MessageHelper::MessageWriter> _writerPool;
};

#endif