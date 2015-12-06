#ifndef POOLOBJECT_H
#define POOLOBJECT_H
#include <assert.h>
#include "../thread/Mutex.h"

struct PoolNode
{
	PoolNode* _prev;
	PoolNode* _next;
	int _size;
	int _used;
};

struct FreeNode
{
	PoolNode* _parent;
	FreeNode* _prev;
	FreeNode* _next;
};

template <class T, int ALLOC_COUNT = 64>
class PoolObject
{
public:
	static void* operator new(size_t size)
	{
		_mutex.lock();

		if (!_freelist)
			AllocPool();

		FreeNode* result = _freelist;
		_freelist = _freelist->_next;

		result->_prev = result->_next = NULL;
		result->_parent->_used++;

		_usedNode++;

		_mutex.unlock();

		return (void*)((char*)result+sizeof(PoolNode*));
	}

	static void	operator delete(void* obj)
	{
		_mutex.lock();

		FreeNode* node = (FreeNode*)((char*)obj - sizeof(PoolNode*));
		node->_parent->_used--;
		node->_prev = NULL;
		node->_next = _freelist;

		if (_freelist != NULL)
			_freelist->_prev = node;
		_freelist = node;

		_usedNode--;

		if (node->_parent->_used == 0)
		{
			if (_totalNode - _usedNode >= 2 * ALLOC_COUNT)
				FreePool(node->_parent);
		}

		_mutex.unlock();
	}

private:
	static void AllocPool()
	{
		static int NODE_SIZE = NodeSize();
		static int POOL_SIZE = PoolSize();

		PoolNode* pool = (PoolNode*)malloc(POOL_SIZE);
		memset(pool,0,POOL_SIZE);

		pool->_size = ALLOC_COUNT;
		pool->_used = 0;
		pool->_next = _pool;
		pool->_prev = NULL;
		if (_pool != NULL)
			_pool->_prev = pool;
		_pool = pool;

		_freelist = (FreeNode*)((PoolNode*)pool + 1);

		char* fn = (char*)_freelist;

		for (int i=0 ; i < ALLOC_COUNT ; ++i)
		{
			FreeNode* node = (FreeNode*)fn;
			node->_parent = pool;

			if (i + 1 < ALLOC_COUNT)
			{
				FreeNode* next = (FreeNode*)(fn + NODE_SIZE);
				node->_next = next;
				next->_prev = node;
			}
			fn += NODE_SIZE;
		}
		_poolCount++;
		_totalNode += ALLOC_COUNT;
	}

	static void FreePool(PoolNode* pool)
	{
		static int NODE_SIZE = NodeSize();	
	
		assert(pool->_used == 0);

		char* fn = (char*)((PoolNode*)pool + 1);

		for (int i=0 ; i < ALLOC_COUNT; ++i)
		{
			FreeNode* node = (FreeNode*)fn;

			if (node == _freelist)
				_freelist = node->_next;

			if (node->_next != NULL)
				node->_next->_prev = node->_prev;

			if (node->_prev != NULL)
				node->_prev->_next = node->_next;

			fn += NODE_SIZE;
		}

		if (_pool == pool)
			_pool = pool->_next;

		if (pool->_next != NULL)
			pool->_next->_prev = pool->_prev;

		if (pool->_prev != NULL)
			pool->_prev->_next = pool->_next;

		free(pool);

		_poolCount--;
		_totalNode -= ALLOC_COUNT;
	}
	
	static int NodeSize()
	{
		static int OBJECT_SIZE = sizeof(T) >= (2 * sizeof(FreeNode*)) ? sizeof(T) : (2 * sizeof(FreeNode*));
		static int NODE_SIZE = OBJECT_SIZE + sizeof(PoolNode*);
		return NODE_SIZE;
	}

	static int PoolSize()
	{
		static int POOL_SIZE = sizeof(PoolNode) + NodeSize() * ALLOC_COUNT;
		return POOL_SIZE;
	}
private:
	static FreeNode*	 _freelist;
	static PoolNode*	 _pool;
	static Thread::Mutex _mutex;
	static int			 _poolCount;
	static int			 _totalNode;
	static int			 _usedNode;
};


template <class TOBJECT, int ALLOC_COUNT>
FreeNode* PoolObject<TOBJECT, ALLOC_COUNT>::_freelist = NULL ;

template <class TOBJECT, int ALLOC_COUNT>
PoolNode* PoolObject<TOBJECT, ALLOC_COUNT>::_pool = NULL ;

template <class TOBJECT, int ALLOC_COUNT>
Thread::Mutex PoolObject<TOBJECT, ALLOC_COUNT>::_mutex;

template <class TOBJECT, int ALLOC_COUNT>
int PoolObject<TOBJECT, ALLOC_COUNT>::_poolCount = 0;

template <class TOBJECT, int ALLOC_COUNT>
int PoolObject<TOBJECT, ALLOC_COUNT>::_totalNode = 0;

template <class TOBJECT, int ALLOC_COUNT>
int PoolObject<TOBJECT, ALLOC_COUNT>::_usedNode = 0;

#endif