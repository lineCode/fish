#ifndef ALLOCATOR_H
#define ALLOCATOR_H


class Allocator
{
public:
	struct PoolNode
	{
		PoolNode* _prev;
		PoolNode* _next;
		int _size;
		int _used;
	};

	struct FreeNode
	{
		PoolNode* _parent;
		FreeNode* _prev;
		FreeNode* _next;
	};
public:
	Allocator(int size,int nodeCount = 64);

	~Allocator();

	void* New();

	void Delete(void* ptr);

	int Dump();

private:
	void CreatePool();

	void FreePool(PoolNode* pool);

private:
	PoolNode* _pool;
	FreeNode* _freelist;
	int _poolCount;
	int _nodeCount;
	int _totalNode;
	int _usedNode;
	int _objectSize;
	int _nodeSize;
	int _poolSize;
};



#endif
