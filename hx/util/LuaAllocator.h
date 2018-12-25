#ifndef MEMALLOC_H
#define MEMALLOC_H
#include "Allocator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

class LuaAllocator
{
public:
	LuaAllocator();

	~LuaAllocator();

	void Dump(int* smallMem,int* big,int* total);

	static void* Alloc(void* ud,void* ptr,size_t osize,size_t nsize);

private:
	void *ExtendSmall(void* ptr,size_t osize,size_t nsize);

	void* AllocSmall(int index);

	void FreeSmall(void* ptr,int index);

private:
	Allocator* _freelist[7];
	int _bigtotal;
	int _memtotal;
};


#endif