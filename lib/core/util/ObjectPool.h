#ifndef OBJECTPOOLX_H
#define OBJECTPOOLX_H

#include <list>
#include <string>
#include "format.h"
#include "List.h"
#include "../thread/Mutex.h"
#include "../Logger.h"

#define OBJ_POOL_INIT_SIZE 16
#define OBJ_POOL_WARNING_SIZE 1024

template<typename T>
class ObjectPool
{
public:
	typedef List<T> OBJECTS;

public:
	ObjectPool(std::string name):
		_objects(),
			_objectName(name),
			_objectCount(0),
			_objectAllocCount(0)
		{
			Thread::MutexGuard guard(_mutex);
			AssignObjs();
		}

		~ObjectPool(void)
		{

		}

		void Pop(T*& obj)
		{
			Thread::MutexGuard guard(_mutex);
			for (;;)
			{
				if (_objects._size > 0)
				{
					_objects.PopHead(obj);
					--_objectCount;
					return;
				}
				AssignObjs();
			}
		}

		void Push(T* obj)
		{
			Thread::MutexGuard guard(_mutex);
			_objects.PushTail(obj);
			_objectCount++;
		}

		void AssignObjs(int size = OBJ_POOL_INIT_SIZE)
		{
			for(int i=0; i<size; ++i)
			{
				_objects.PushTail(new T);
				++_objectCount;
				++_objectAllocCount;
				if (_objectAllocCount >= OBJ_POOL_WARNING_SIZE)
					LOG_ERROR(fmt::format("pool object:{} allocate too much:{}",_objectName,_objectAllocCount));
			}
		}

private:
	OBJECTS _objects;
	std::string _objectName;
	int _objectCount;
	int _objectAllocCount;
	Thread::Mutex _mutex;
};


#endif