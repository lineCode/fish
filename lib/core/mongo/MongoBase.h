#ifndef MONGOBASE_H
#define MONGOBASE_H
#include <map>
#include <string>
#include "boost/function.hpp"
#include "../network/Session.h"
#include "../util/MemoryStream.h"
#include "../util/List.h"
#include "../Typedef.h"
#include "bson.h"

#define DEFAULT_CAP 128
typedef void * document;

struct MongoBuffer
{
	int		_offset;
	int		_cap;
	char*	_ptr;
	char	_buffer[DEFAULT_CAP];

	MongoBuffer()
	{
		_offset = 0;
		_cap = DEFAULT_CAP;
		_ptr = _buffer;
	}

	~MongoBuffer()
	{
		if (_ptr != _buffer)
			free(_ptr);
	}

	int size()
	{
		return _offset;
	}

	char* ptr()
	{
		return _ptr;
	}

	template<typename T>
	MongoBuffer& append(T value)
	{
		int cnt = sizeof(value);
		reserve(cnt+1);
		memcpy(&_ptr[_offset],(void*)&value,cnt);
		_offset += cnt;

		return *this;
	}

	MongoBuffer& append(const char* c,size_t cnt) 
	{
		if (cnt != 0)
		{
			reserve(cnt+1);
			memcpy(&_ptr[_offset],c,cnt);
			_offset += cnt;

			_ptr[_offset++] = 0;
		}
		return *this;
	}

	void reserve(int size)
	{
		if (_offset + size <= _cap)
			return;
		do 
		{
			_cap *= 2;
		} 
		while (_cap <= _offset + size);

		if (_ptr == _buffer) 
		{
			_ptr = (char*)malloc(_cap);
			memcpy(_ptr, _buffer, _offset);
		} 
		else
			_ptr = (char*)realloc(_ptr, _cap);
	}

	int reserveLength()
	{
		int sz = _offset;
		reserve(4);
		_offset +=4;
		return sz;
	}

	void writeLength(int32 v, int off)
	{
		uint32 uv = (uint32)v;
		_ptr[off++] = uv & 0xff;
		_ptr[off++] = (uv >> 8)&0xff;
		_ptr[off++] = (uv >> 16)&0xff;
		_ptr[off++] = (uv >> 24)&0xff;
	}
};

class MongoBase : public Network::Session
{
public:
	enum OPCODE
	{
		OP_REPLY = 1,
		OP_MSG = 1000,
		OP_UPDATE = 2001,
		OP_INSERT = 2002,
		OP_QUERY = 2004,
		OP_GET_MORE = 2005,
		OP_DELETE = 2006,
		OP_KILL_CURSORS = 2007,
	};

	struct ReplyMsg
	{
		//int32 length;
		int32 requestId;
		int32 responseId;
		int32 opcode;
		int32 flags;
		int32 cursorId[2];
		int32 starting;
		int32 number;
	};

	enum QueryResult 
	{
		REPLY_CURSORNOTFOUND = 1,
		REPLY_QUERYFAILURE = 2,
		REPLY_AWAITCAPABLE = 8,
	};

	typedef boost::function<void (const char*,int)>	CallBackFunc;
	typedef FishMap<int,CallBackFunc> QueryCallBack;
	typedef FishMap<int,std::string> QueryCollection;
	typedef FishMap<int,MemoryStream*> QueryResults;
	typedef List<MemoryStream> StreamList;
public:
	MongoBase(Network::EventPoller * poller,int fd);

	~MongoBase(void);
	
	virtual int Init();

	virtual int Fina();

	virtual int Forward(const char * ptr,int size);

	virtual int Dispatcher(int reponseId,char* data,int size);

	int Reply(const char* ptr,int size);

	int RunCommand(CallBackFunc cb,bson::BSONObj& query);

	int Query(CallBackFunc cb,std::string name,bson::BSONObj& query,bson::BSONObj& selector,int flag = 0,int skip = 0,int number = 100);

	int Update(std::string name,int flag,bson::BSONObj& selector,bson::BSONObj& updator);

	int Insert(std::string name,bson::BSONObj& doc,int flag);

	int doRunCommand(const char* name,int nameSize,const char* cmd,int cmdSize);

	int doQuery(const char* name,int size,const char* query,int querySize,const char* selector,int selectorSize,int flag = 0,int skip = 0,int number = 100);
	
	int doMore(const char* name,int size,int number,const char*cursor,int session = 0);

	int doUpdate(const char* name,int size,int flag,const char* selector,int selectorSize,const char* updator,int updatorSize);

	int doInsert(const char* name,int size,int flag,const char* doc,int docSize);

	void PopStream(MemoryStream*& ms);

	void PushStream(MemoryStream* ms);
private:
	int	_session;
	QueryCallBack _queryCallBack;
	QueryCollection _queryCollection;
	QueryResults _queryResult;
	StreamList	_waitlist;
	StreamList	_freelist;
};

#endif