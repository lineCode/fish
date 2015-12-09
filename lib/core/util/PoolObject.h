#ifndef POOLOBJECT_H
#define POOLOBJECT_H
#include <assert.h>
#include "../thread/Mutex.h"
#include "Allocator.h"

template <class T, int ALLOC_COUNT = 64>
class PoolObject
{
public:
	static void* operator new(size_t size)
	{
		if (_allocator == NULL)
			_allocator = new Allocator(size,ALLOC_COUNT);
		return _allocator->New();
	}

	static void	operator delete(void* obj)
	{
		_allocator->Delete(obj);
	}

private:
	static Allocator* _allocator;
};


template <class TOBJECT, int ALLOC_COUNT>
Allocator* PoolObject<TOBJECT, ALLOC_COUNT>::_allocator = NULL ;

#endif