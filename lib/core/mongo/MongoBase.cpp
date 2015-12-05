#include <assert.h>
#include "MongoBase.h"
#include "MongoCursor.h"
#include "../util/MemoryStream.h"
#include "../Logger.h"

MongoBase::MongoBase(Network::EventPoller * poller,int fd):Network::Session(poller,fd)
{
	_session = 0;
}


MongoBase::~MongoBase(void)
{
	while (_freelist.Empty() == false)
	{
		MemoryStream* ms;
		_freelist.PopHead(ms);
		delete ms;
	}

	while(_waitlist.Empty() == false)
	{
		MemoryStream* ms;
		_waitlist.PopHead(ms);
		delete ms;
	}
	delete _reader;
}

int MongoBase::Init()
{
	this->_reader = new Network::TcpReader(this,4,1024*1024);
	return 0;
}

int MongoBase::Fina()
{
	return 0;
}

int MongoBase::Forward(const char * ptr,int size)
{
	return this->Reply(ptr,size);
}

int MongoBase::Dispatcher(int reponseId,char* data,int size)
{
	QueryCallBack::iterator iter = _queryCallBack.find(reponseId);
	if (iter != _queryCallBack.end())
	{
		iter->second(data,size);
		_queryCallBack.erase(iter);
		return 0;
	}
	return -1;
}

int MongoBase::Reply(const char* ptr,int size)
{
	static int ReplyMsgSize = sizeof(ReplyMsg);

	assert(size >= ReplyMsgSize);

	ReplyMsg* reply = (ReplyMsg*)ptr;

	if (reply->flags & REPLY_QUERYFAILURE)
	{
		LOG_ERROR(fmt::format("mongo query failed:{}",reply->flags));
		this->Dispatcher(reply->responseId,(char*)((ReplyMsg*)ptr + 1),size - ReplyMsgSize);
	}
	else
	{
		if (reply->cursorId[0] != 0 || reply->cursorId[1] != 0)
		{
			//need more
			std::string& collection = _queryCollection[reply->responseId];
			MemoryStream* stream = _queryResult[reply->responseId];
			if (stream == NULL)
			{
				this->PopStream(stream);
				_queryResult[reply->responseId] = stream;
			}
			(*stream).append((uint8*)((ReplyMsg*)ptr + 1),size - ReplyMsgSize);
			this->doMore(collection.c_str(),collection.length(),10000,(const char*)reply->cursorId,reply->responseId);
		}
		else
		{
			MemoryStream* stream = _queryResult[reply->responseId];
			if (stream == NULL)
			{
				if (size > 1024*1024*10)
					LOG_ERROR(fmt::format("mongo query reply too much:{}kb",size / 1024));
				this->Dispatcher(reply->responseId,(char*)((ReplyMsg*)ptr + 1),size - ReplyMsgSize);
			}
			else
			{
				(*stream).append((uint8*)((ReplyMsg*)ptr + 1),size - ReplyMsgSize);
				if (stream->length() > 1024*1024*10)
					LOG_ERROR(fmt::format("mongo query reply too much:{}kb",stream->length() / 1024));

				this->Dispatcher(reply->responseId,stream->begin(),stream->length());
				_queryResult.erase(reply->responseId);
				this->PushStream(stream);
			}
			_queryCollection.erase(reply->responseId);
		}
	}

	free((void*)ptr);

	return 0;
}

void MongoBase::QueryReply(MongoQuery* reply,const char* data,int size)
{
	reply->Reply((char*)data,size);
	delete reply;
}

int MongoBase::RunCommand(MongoQuery* reply,bson::BSONObj& query)
{
	bson::BSONObj selector;
	return this->Query(reply,std::string("admin.$cmd"),query,selector);
}

int MongoBase::Query(MongoQuery* reply,std::string name,bson::BSONObj& query,bson::BSONObj& selector,int flag /* = 0 */,int skip /* = 0 */,int number /* = 100 */)
{
	int session = this->doQuery(name.c_str(),name.size(),query.objdata(),query.objsize(),selector.objdata(),selector.objsize(),flag,skip,number);
	_queryCallBack[session] = boost::bind(&MongoBase::QueryReply,this,reply,_1,_2);
	return session;
}

int MongoBase::Update(std::string name,int flag,bson::BSONObj& selector,bson::BSONObj& updator)
{
	return this->doUpdate(name.c_str(),name.size(),flag,selector.objdata(),selector.objsize(),updator.objdata(),updator.objsize());
}

int MongoBase::Insert(std::string name,bson::BSONObj& doc,int flag /* = 0 */)
{
	return this->doInsert(name.c_str(),name.size(),flag,doc.objdata(),doc.objsize());
}

int MongoBase::doRunCommand(const char* name,int size,const char* cmd,int cmdSize)
{
	bson::BSONObj bo;
	return this->doQuery(name,size,cmd,cmdSize,bo.objdata(),bo.objsize(),0,0,1);
}

int MongoBase::doQuery(const char* name,int size,const char* query,int querySize,const char* selector,int selectorSize,int flag,int skip,int number)
{
	int session = ++_session;

	MongoBuffer buff;

	int32 len = buff.reserveLength();

	buff.append(session).append((int32)0).append((int32)OP_QUERY).append((int32)flag).append(name,size).append((int32)skip).append((int32)number);

	int total = buff.size() + querySize + selectorSize;

	buff.writeLength(total,len);

	char * data = (char*)malloc(total);
	memcpy(data,(const void*)buff.ptr(),buff.size());
	memcpy(data + buff.size(),query,querySize);
	memcpy(data+buff.size() + querySize,selector,selectorSize);

	this->Send(data,total);

	_queryCollection[session] = name;
	return session;
}

int MongoBase::doMore(const char* name,int size,int number,const char*cursor,int session)
{
	if (session == 0)
		session = ++_session;

	MongoBuffer buff;

	int32 len = buff.reserveLength();

	buff.append(session).append((int32)0).append((int32)OP_GET_MORE).append((int32)0).append(name,size).append((int32)number).append(cursor,8);

	buff.writeLength(buff.size(),len);

	char * data = (char*)malloc(buff.size());
	memcpy(data,(const void*)buff.ptr(),buff.size());

	this->Send(data,buff.size());

	return session;
}

int MongoBase::doUpdate(const char* name,int size,int flag,const char* selector,int selectorSize,const char* updator,int updatorSize)
{
	MongoBuffer buff;

	int32 len = buff.reserveLength();

	buff.append((int32)0).append((int32)0).append((int32)OP_UPDATE).append((int32)0).append(name,size).append((int32)flag);

	int total = buff.size() + selectorSize + updatorSize;

	buff.writeLength(total,len);

	char * data = (char*)malloc(total);

	memcpy(data,(const void*)buff.ptr(),buff.size());
	memcpy(data + buff.size(),selector,selectorSize);
	memcpy(data + buff.size() + selectorSize,updator,updatorSize);

	this->Send(data,total);
	return 0;
}

int MongoBase::doInsert(const char* name,int size,int flag,const char* doc,int docSize)
{
	MongoBuffer buff;

	int32 len = buff.reserveLength();

	buff.append((int32)0).append((int32)0).append((int32)OP_INSERT).append((int32)flag).append(name,size);

	int total = buff.size() + docSize;

	buff.writeLength(total,len);

	char * data = (char*)malloc(total);
	memcpy(data,(const void*)buff.ptr(),buff.size());
	memcpy(data+buff.size(),doc,docSize);

	this->Send(data,total);
	return 0;
}

void MongoBase::PopStream(MemoryStream*& ms)
{
	ms = NULL;
	if (_waitlist.Empty() == false)
	{
		ms = _waitlist.Back();
		if ((int)ms->size() - ms->wpos() > 0)
			return;
	}

	if (_freelist.Empty())
		ms = new MemoryStream();
	else
	{
		ms = _freelist.Front();
		_freelist.RemoveFront();
	}
	_waitlist.PushTail(ms);

	return;
}

void MongoBase::PushStream(MemoryStream* ms)
{
	ms->reset();
	_freelist.PushTail(ms);
}

