#include "Allocator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

Allocator::Allocator(int size,int nodeCount)
{
	_pool = NULL;
	_freelist = NULL;
	_poolCount = 0;
	_nodeCount = nodeCount;
	_totalNode = 0;
	_usedNode = 0;

	if (size <= 2 * sizeof(FreeNode*))
		size = 2 * sizeof(FreeNode*);
	_objectSize = size;
	_nodeSize = _objectSize + sizeof(PoolNode*);
	_poolSize = sizeof(PoolNode) + _nodeSize * _nodeCount;
}

Allocator::~Allocator()
{
	PoolNode* pool = _pool;
	while(pool != NULL)
	{
		PoolNode* tmp = pool;
		pool = pool->_next;
		if (tmp->_used == 0)
			free(tmp);
	}
}

void* Allocator::New()
{
	if (!_freelist)
		CreatePool();

	FreeNode* ptr = _freelist;
	_freelist = _freelist->_next;
	ptr->_prev = ptr->_next = NULL;
	ptr->_parent->_used++;

	_usedNode++;

	return (void*)((char*)ptr+sizeof(PoolNode*));
}

void Allocator::Delete(void* ptr)
{
	FreeNode* fn = (FreeNode*)((char*)ptr - sizeof(PoolNode*));
	fn->_parent->_used--;
	fn->_prev = NULL;
	fn->_next = _freelist;

	if (_freelist != NULL)
		_freelist->_prev = fn;
	_freelist = fn;

	_usedNode--;

	//if (fn->_parent->_used == 0)
	//{
	//	if (_totalNode - _usedNode >= 2 * _nodeCount)
	//		FreePool(fn->_parent);
	//}
		
}

void Allocator::CreatePool()
{
	PoolNode* pool = (PoolNode*)malloc(_poolSize);
	memset(pool,0,_poolSize);

	pool->_size = _nodeCount;
	pool->_used = 0;
	pool->_next = _pool;
	pool->_prev = NULL;
	if (_pool != NULL)
		_pool->_prev = pool;
	_pool = pool;

	_freelist = (FreeNode*)((PoolNode*)pool + 1);

	char* fn = (char*)_freelist;

	for (int i=0 ; i < _nodeCount ; ++i)
	{
		FreeNode* node = (FreeNode*)fn;
		node->_parent = pool;

		if (i + 1 < _nodeCount)
		{
			FreeNode* next = (FreeNode*)(fn + _nodeSize);
			node->_next = next;
			next->_prev = node;
		}
		
		fn += _nodeSize;
	}
	_poolCount++;
	_totalNode += _nodeCount;
}

void Allocator::FreePool(PoolNode* pool)
{
	assert(pool->_used == 0);
	char* fn = (char*)((PoolNode*)pool + 1);

	for (int i = 0; i < _nodeCount;i++)
	{
		FreeNode* node = (FreeNode*)fn;

		if (node == _freelist)
			_freelist = node->_next;

		if (node->_next != NULL)
			node->_next->_prev = node->_prev;

		if (node->_prev != NULL)
			node->_prev->_next = node->_next;

		fn += _nodeSize;
	}

	if (_pool == pool)
		_pool = pool->_next;

	if (pool->_next != NULL)
		pool->_next->_prev = pool->_prev;

	if (pool->_prev != NULL)
		pool->_prev->_next = pool->_next;

	free(pool);

	_poolCount--;
	_totalNode -= _nodeCount;
}

int Allocator::Dump()
{
	return _poolCount * _poolSize;
}