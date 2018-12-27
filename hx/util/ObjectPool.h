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
	ObjectPool(std::string name): objects_(), name_(name), count_(0), cap_(0) {
		AssignObjs();
	}

	~ObjectPool(void) {
		while(objects_.Empty() == false) {
			T* obj;
			objects_.PopHead(obj);
			delete obj;
        }
	}

	void Pop(T*& obj) {
		for (;;) {
			if (objects_._size > 0) {
				objects_.PopHead(obj);
				--count_;
				return;
			}
			AssignObjs();
		}
	}

	void Push(T* obj) {
		objects_.PushTail(obj);
		count_++;
	}

	void AssignObjs(int size = OBJ_POOL_INIT_SIZE) {
		for(int i=0; i<size; ++i) {
			objects_.PushTail(new T);
			++count_;
			++cap_;
			if (cap_ >= OBJ_POOL_WARNING_SIZE) {
				LOG_ERROR(fmt::format("pool object:{} allocate too much:{}",name_,cap_));
			}
		}
	}

private:
	OBJECTS objects_;
	std::string name_;
	int count_;
	int cap_;
};


#endif