#include "MemAlloc.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define CHUNK_SIZE(size) (size+sizeof(Memchunk))

MemAlloc::MemAlloc(int size)
{
	for (int i = 0;i < 7;i++)
		this->_freelist[i] = NULL;
	_chunklist = NULL;
	_chunksize = size;
	_ptr = _end = NULL;
	_chunktoal = _bigtotal = _memtotal = 0;
	CreateChunk();
}

MemAlloc::~MemAlloc()
{
	Memchunk* mc = this->_chunklist;
	while (mc)
	{
		Memchunk* tmp = mc;
		mc = mc->_next;
		free((void*)tmp);
	}
}

void MemAlloc::CreateChunk()
{
	Memchunk* mc = (Memchunk*)malloc(CHUNK_SIZE(_chunksize));
	mc->_next = this->_chunklist;
	this->_ptr = (char*)(mc+1);
	this->_end = ((char*)mc) + CHUNK_SIZE(_chunksize);
	this->_chunklist = mc;

	_chunktoal++;
}

void MemAlloc::Info(int* smallTotal,int* bigTotal,int *memTotal)
{
	*smallTotal = _chunktoal * CHUNK_SIZE(_chunksize);
	*bigTotal = _bigtotal;
	*memTotal = _memtotal;
}

void* MemAlloc::ExtendSmall(void* ptr,size_t osize,size_t nsize)
{
	int oidx = SizeIndex(osize);
	int nidx = SizeIndex(nsize);
	if (oidx == nidx)
		return ptr;
	
	void* result = AllocSmall(nidx);
	memcpy(result,ptr,osize < nsize ? osize:nsize);
	FreeSmall(ptr,oidx);
	return result;
}

void* MemAlloc::AllocSmall(int index)
{
	Freenode* fn = this->_freelist[index];
	if (fn)
	{
		this->_freelist[index] = fn->_next;
		return fn;
	}

	int rsz = 1 << (index + 3);
	if ((this->_end - this->_ptr) < rsz)
		CreateChunk();

	void* result = this->_ptr;
	this->_ptr += rsz;
	return result;
	
}

void MemAlloc::FreeSmall(void* ptr,int index)
{
	Freenode* fn = _freelist[index];
	Freenode* node = (Freenode*)ptr;
	node->_next = fn;
	_freelist[index] = node;
}

int MemAlloc::SizeIndex(size_t sz)
{
	if (sz > 32)
	{
		if (sz > 128)
		{
			if (sz > 256)
				return 6;
			else
				return 5;	
		}
		else
		{
			if (sz > 64)
				return 4;
			else
				return 3;
		}
	}
	else
	{
		if (sz > 16)
			return 2;
		else if (sz > 8)
			return 1;
		else
			return 0;
	}
	assert(0);
	return -1;
}

void* MemAlloc::Alloc(void* ud,void* ptr,size_t osize,size_t nsize)
{
	MemAlloc* self = (MemAlloc*)ud;
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
		
		int index = MemAlloc::SizeIndex(nsize);
		return self->AllocSmall(index);
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
		self->FreeSmall(ptr,SizeIndex(osize));
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
				int idx = SizeIndex(nsize);
				void* result = self->AllocSmall(idx);
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
			self->FreeSmall(ptr,SizeIndex(osize));
			return result;
		}
		else
			return self->ExtendSmall(ptr,osize,nsize);
	}
	assert(false);
	return NULL;
}