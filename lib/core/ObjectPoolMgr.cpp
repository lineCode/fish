#include "ObjectPoolMgr.h"

template <> 
ObjectPoolMgr * Singleton<ObjectPoolMgr>::singleton_ = 0;

ObjectPoolMgr::ObjectPoolMgr(void):
	_poolReader("MessageReader"),_poolWriter("MessageWriter")
{
}


ObjectPoolMgr::~ObjectPoolMgr(void)
{
}

ObjectPool<MessageHelper::MessageReader>& ObjectPoolMgr::MesasgeReaderPool()
{
	return _poolReader;
}

ObjectPool<MessageHelper::MessageWriter>& ObjectPoolMgr::MessageWriterPool()
{
	return _poolWriter;
}
