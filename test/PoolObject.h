#ifndef POOLOBJECT_H
#define POOLOBJECT_H
#include <assert.h>
#include <stdio.h>
#include "Allocator.h"

template <class T, int ALLOC_COUNT = 64>
class PoolObject
{
public:
	static void* operator new(size_t size)
	{
		assert(sizeof( T ) == size);
		return _allocator.New();
	}

	static void	operator delete(void* obj)
	{
		_allocator.Delete(obj);
	}

	static int Dump() {
		return _allocator.Dump();
	}
private:
	static Allocator _allocator;
};


template <class T, int ALLOC_COUNT>
Allocator PoolObject<T, ALLOC_COUNT>::_allocator(T::GetClassName(), sizeof( T ), ALLOC_COUNT);

#endif