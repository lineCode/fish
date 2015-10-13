#ifndef MONGOBASE_H
#define MONGOBASE_H
#include <map>
#include <string>
#include "boost/function.hpp"
#include "../network/Session.h"
#include "../util/MemoryStream.h"
#include "../util/List.h"
#include "bson.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
};

#define DEFAULT_CAP 128
typedef void * document;

struct MongoBuffer
{
	int		_size;
	int		_cap;
	char*	_ptr;
	char	_buffer[DEFAULT_CAP];

	MongoBuffer()
	{
		_size = 0;
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
		return _size;
	}

	char* ptr()
	{
		return _ptr;
	}

	template<typename T>
	void append(T value)
	{
		append((const uint8 *)&value, sizeof(value));
	}

	void append(const char* c,size_t cnt) 
	{
		append((const uint8*)c,cnt);
		append((uint8)0);
	}

	void append(const std::string& str)
	{
		append(str.c_str(),str.size());
		append((uint8)0);
	}

	void append(const uint8* val,size_t cnt) 
	{
		if (cnt == 0)
			return;

		reserve(cnt);
		memcpy(&_ptr[_size],val,cnt);
		_size += cnt;
	}

	void reserve(int size)
	{
		if (_size + size <= _cap)
			return;
		do 
		{
			_cap *= 2;
		} 
		while (_cap <= _size + size);

		if (_ptr == _buffer) 
		{
			_ptr = (char*)malloc(_cap);
			memcpy(_ptr, _buffer, _size);
		} 
		else
			_ptr = (char*)realloc(_ptr, _cap);
	}

	int reserveLength()
	{
		int sz = _size;
		reserve(4);
		_size +=4;
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
	typedef std::map<int,CallBackFunc> QueryCallBack;
	typedef std::map<int,std::string> QueryCollection;
	typedef std::map<int,MemoryStream*> QueryResults;
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