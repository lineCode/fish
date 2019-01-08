#ifndef POOLOBJECT_H
#define POOLOBJECT_H
#include <assert.h>
#include "Allocator.h"

template <class T, int ALLOC_COUNT = 64>
class PoolObject
{
public:
	static void* operator new(size_t size) {
		assert(sizeof( T ) == size);
		return allocator_.New();
	}

	static void	operator delete(void* ptr) {
		allocator_.Delete(ptr);
	}

	static void DumpPool() {
		allocator_.Dump();
	}

private:
	static Allocator allocator_;
};


template <class T, int ALLOC_COUNT>
Allocator PoolObject<T, ALLOC_COUNT>::allocator_(T::GetClassName(), sizeof( T ), ALLOC_COUNT);

#endif