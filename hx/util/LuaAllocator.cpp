#include "LuaAllocator.h"


static int where[] = {
  0,0,0,0,0,0,0,0,0,//8
	1,1,1,1,1,1,1,1,//8
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,//16
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,//32
	4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,//64
	5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,//128
	6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6//256
};

LuaAllocator::LuaAllocator()
{
	for (int i = 0;i < 7;i++)
		this->_freelist[i] = NULL;
	_bigtotal = _memtotal = 0;
}

LuaAllocator::~LuaAllocator()
{
	for (int i = 0;i < 7;i++)
	{
		Allocator* allocator = this->_freelist[i];
		if (allocator != NULL)
			delete allocator;
	}
		
}

void LuaAllocator::Dump(int* smallMem,int* big,int* total)
{
	int smallTotal = 0;
	for (int i = 0;i < 7;i++)
	{
		Allocator* allocator = _freelist[i];
		if (allocator)
			smallTotal += allocator->Dump();
	}
	*smallMem = smallTotal;
	*big = _bigtotal;
	*total = _memtotal;
}

void *LuaAllocator::ExtendSmall(void* ptr,size_t osize,size_t nsize)
{
	int oidx = where[osize];
	int nidx = where[nsize];
	if (oidx == nidx)
		return ptr;
	
	void* result = AllocSmall(nidx);
	memcpy(result,ptr,osize < nsize ? osize:nsize);
	FreeSmall(ptr,oidx);
	return result;
}

void* LuaAllocator::AllocSmall(int index)
{
	int rsz = 1 << (index + 3);
	Allocator* allocator = this->_freelist[index];
	if (!allocator)
	{
		allocator = new Allocator(rsz,128);
		this->_freelist[index] = allocator;
	}
	return allocator->New();
}

void LuaAllocator::FreeSmall(void* ptr,int index)
{
	Allocator* allocator = this->_freelist[index];
	allocator->Delete(ptr);
}

void* LuaAllocator::Alloc(void* ud,void* ptr,size_t osize,size_t nsize)
{
	LuaAllocator* self = (LuaAllocator*)ud;
	if (ptr == NULL)
	{
		self->_memtotal += nsize;
		if (nsize > 512)
		{
			self->_bigtotal += nsize;
			return malloc(nsize);
		}

		if (nsize == 0)
			return NULL;
		
		return self->AllocSmall(where[nsize]);
	}
	else if (nsize == 0)
	{
		self->_memtotal -= osize;
		if (osize > 512)
		{
			self->_bigtotal -= osize;
			free(ptr);
			return NULL;
		}
		self->FreeSmall(ptr,where[osize]);
		return NULL;
	}
	else
	{
		self->_memtotal -= osize;
		self->_memtotal += nsize;

		if (osize > 512)
		{
			if (nsize > 512)
			{
				self->_bigtotal -= osize;
				self->_bigtotal += nsize;
				return realloc(ptr,nsize);
			}
			else
			{
				self->_bigtotal -= osize;
				void* result = self->AllocSmall(where[nsize]);
				memcpy(result,ptr,nsize);
				free(ptr);
				return result;
			}
		}

		if (nsize > 512)
		{
			self->_bigtotal += nsize;
			void* result = malloc(nsize);
			memcpy(result,ptr,osize);
			self->FreeSmall(ptr,where[osize]);
			return result;
		}
		else
			return self->ExtendSmall(ptr,osize,nsize);
	}
	assert(false);
	return NULL;
}
