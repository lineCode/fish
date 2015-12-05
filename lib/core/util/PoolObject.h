#ifndef POOLOBJECT_H
#define POOLOBJECT_H
#include "../Typedef.h"
#include "../thread/Mutex.h"

struct Pool
{
	struct Pool* _next;
};

template <class TOBJECT, int ALLOC_COUNT=10>
class PoolObject
{
public:
	static void* operator new(size_t objSize)
	{
		_mutex.lock();

		if (!_freelist)
		{
			Pool* pool = (Pool*)malloc(sizeof(Pool) + sizeof(TOBJECT)*ALLOC_COUNT);
			pool->_next = _pool;
			_pool = pool;
			
			_freelist = (uint8*)((Pool*)pool + 1);

			uint8* next = _freelist ;
			uint8** curr = reinterpret_cast<uint8**>(_freelist) ;

			for (int i=0 ; i<ALLOC_COUNT-1 ; ++i)
			{
				next += sizeof(TOBJECT) ;
				*curr = next ;
				curr = reinterpret_cast<uint8**>(next) ;
			}

			*curr = 0 ;
			_allocCount += ALLOC_COUNT ;
		}

		uint8* result = _freelist ;
		_freelist = *reinterpret_cast<uint8**>(result) ;
		++_useCount ;

		_mutex.unlock();

		return result;
	}

	static void	operator delete(void* obj)
	{
		_mutex.lock();

		--_useCount ;

		*reinterpret_cast<uint8**>(obj) = _freelist ;
		_freelist = static_cast<uint8*>(obj) ;

		_mutex.unlock();
	}


private:
	static uint8*		 _freelist;
	static Pool*		 _pool;
	static int			 _allocCount ;
	static int			 _useCount ;
	static Thread::Mutex _mutex;
};


template <class TOBJECT, int ALLOC_COUNT>
uint8* PoolObject<TOBJECT, ALLOC_COUNT>::_freelist = NULL ;

template <class TOBJECT, int ALLOC_COUNT>
Pool* PoolObject<TOBJECT, ALLOC_COUNT>::_pool = NULL ;

template <class TOBJECT, int ALLOC_COUNT>
int PoolObject<TOBJECT, ALLOC_COUNT>::_allocCount = 0 ;

template <class TOBJECT, int ALLOC_COUNT>
int PoolObject<TOBJECT, ALLOC_COUNT>::_useCount = 0 ;

template <class TOBJECT, int ALLOC_COUNT>
Thread::Mutex PoolObject<TOBJECT, ALLOC_COUNT>::_mutex;

#endif