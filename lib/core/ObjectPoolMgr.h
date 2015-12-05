#ifndef OBJECTPOOLMGR_H
#define OBJECTPOOLMGR_H

#include "util/Singleton.h"
#include "util/ObjectPool.h"
#include "util/MessageReader.h"
#include "util/MessageWriter.h"

class ObjectPoolMgr : public Singleton<ObjectPoolMgr>
{
public:
	ObjectPoolMgr(void);
	~ObjectPoolMgr(void);

	ObjectPool<MessageHelper::MessageReader>& MesasgeReaderPool();
	ObjectPool<MessageHelper::MessageWriter>& MessageWriterPool();

private:
	ObjectPool<MessageHelper::MessageReader> _poolReader;
	ObjectPool<MessageHelper::MessageWriter> _poolWriter;
};

#endif
