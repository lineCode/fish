#ifndef SINGLETON_H
#define SINGLETON_H
#include <assert.h>

template <typename T> 
class Singleton
{
protected:
	static T* singleton_;

public:
	Singleton(void)
	{
		assert(!singleton_);
		singleton_ = static_cast< T* >(this);
	}


	~Singleton(void){  assert(singleton_);  singleton_ = 0; }

	static T& GetSingleton(void) { assert(singleton_);  return (*singleton_); }
	static T* GetSingletonPtr(void){ return singleton_; }
};

#endif