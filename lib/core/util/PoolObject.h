#ifndef POOLOBJECT_H
#define POOLOBJECT_H
#include "../Typedef.h"
#include "../thread/Mutex.h"

template <class TOBJECT, int ALLOC_COUNT=10>
class PoolObject
{
public:

	static void* operator new(size_t objSize)
	{
		mMutex.lock();

		if (!mFreeList)
		{
			mFreeList = new uint8[sizeof(TOBJECT)*ALLOC_COUNT] ;

			uint8* pNext = mFreeList ;
			uint8** ppCurr = reinterpret_cast<uint8**>(mFreeList) ;

			for (int i=0 ; i<ALLOC_COUNT-1 ; ++i)
			{
				pNext += sizeof(TOBJECT) ;
				*ppCurr = pNext ;
				ppCurr = reinterpret_cast<uint8**>(pNext) ;
			}

			*ppCurr = 0 ;
			mTotalAllocCount += ALLOC_COUNT ;
		}

		uint8* pAvailable = mFreeList ;
		mFreeList = *reinterpret_cast<uint8**>(pAvailable) ;
		++mCurrentUseCount ;

		mMutex.unlock();

		return pAvailable ;
	}

	static void	operator delete(void* obj)
	{
		mMutex.lock();

		--mCurrentUseCount ;

		*reinterpret_cast<uint8**>(obj) = mFreeList ;
		mFreeList = static_cast<uint8*>(obj) ;

		mMutex.unlock();
	}


private:
	static uint8*		 mFreeList ;
	static int			 mTotalAllocCount ;
	static int			 mCurrentUseCount ;
	static Thread::Mutex mMutex;
};


template <class TOBJECT, int ALLOC_COUNT>
uint8* PoolObject<TOBJECT, ALLOC_COUNT>::mFreeList = NULL ;

template <class TOBJECT, int ALLOC_COUNT>
int PoolObject<TOBJECT, ALLOC_COUNT>::mTotalAllocCount = 0 ;

template <class TOBJECT, int ALLOC_COUNT>
int PoolObject<TOBJECT, ALLOC_COUNT>::mCurrentUseCount = 0 ;

template <class TOBJECT, int ALLOC_COUNT>
Thread::Mutex PoolObject<TOBJECT, ALLOC_COUNT>::mMutex;

#endif