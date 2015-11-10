#ifndef MEMALLOC_H
#define MEMALLOC_H

class MemAlloc
{
public:
	struct Freenode
	{
		struct Freenode* _next;
	};

	struct Memchunk
	{
		struct Memchunk* _next;
	};
public:
	MemAlloc(int size = 1024 * 16);

	~MemAlloc();

	void Info(int* smallTotal,int* bigTotal,int *memTotal);

	void* ExtendSmall(void* ptr,size_t osize,size_t nsize);

	void* AllocSmall(int index);

	void FreeSmall(void* ptr,int index);

	static int SizeIndex(size_t sz);

	static void* Alloc(void* ud,void* ptr,size_t osize,size_t nsize);

private:
	void CreateChunk();

private:
	Freenode* _freelist[7];
	Memchunk* _chunklist;
	int _chunksize;
	char* _ptr;
	char* _end;

	int _chunktoal;
	int _bigtotal;
	int _memtotal;
};


#endif