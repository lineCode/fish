#ifndef ALLOCATOR_H
#define ALLOCATOR_H


class Allocator {
public:
	struct PoolNode {
		PoolNode* prev_;
		PoolNode* next_;
		int size_;
		int used_;
	};

	struct FreeNode {
		PoolNode* parent_;
		FreeNode* prev_;
		FreeNode* next_;
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
	PoolNode* pool_;
	FreeNode* freelist_;
	int poolCount_;
	int nodeCount_;
	int totalNode_;
	int usedNode_;
	int objectSize_;
	int nodeSize_;
	int poolSize_;
};



#endif
