#include "Allocator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

Allocator::Allocator(int size,int nodeCount) {
	pool_ = NULL;
	freelist_ = NULL;
	poolCount_ = 0;
	nodeCount_ = nodeCount;
	totalNode_ = 0;
	usedNode_ = 0;

	if (size <= 2 * (int)sizeof(FreeNode*)) {
		size = 2 * (int)sizeof(FreeNode*);
	}
	objectSize_ = size;
	nodeSize_ = objectSize_ + sizeof(PoolNode*);
	poolSize_ = sizeof(PoolNode) + nodeSize_ * nodeCount_;
}

Allocator::~Allocator() {
	PoolNode* pool = pool_;
	while(pool != NULL) {
		PoolNode* tmp = pool;
		pool = pool->next_;
		if (tmp->used_ == 0) {
			free(tmp);
		}
	}
}

void* Allocator::New() {
	if (!freelist_) {
		CreatePool();
	}

	FreeNode* ptr = freelist_;
	freelist_ = freelist_->next_;
	ptr->prev_ = ptr->next_ = NULL;
	ptr->parent_->used_++;

	usedNode_++;

	return (void*)((char*)ptr+sizeof(PoolNode*));
}

void Allocator::Delete(void* ptr) {
	FreeNode* fn = (FreeNode*)((char*)ptr - sizeof(PoolNode*));
	fn->parent_->used_--;
	fn->prev_ = NULL;
	fn->next_ = freelist_;

	if (freelist_ != NULL) {
		freelist_->prev_ = fn;
	}
	freelist_ = fn;

	usedNode_--;

	//if (fn->_parent->used_ == 0)
	//{
	//	if (totalNode_ - _usedNode >= 2 * nodeCount_)
	//		FreePool(fn->parent_);
	//}
}

void Allocator::CreatePool() {
	PoolNode* pool = (PoolNode*)malloc(poolSize_);
	memset(pool,0,poolSize_);

	pool->size_ = nodeCount_;
	pool->used_ = 0;
	pool->next_ = pool_;
	pool->prev_ = NULL;
	if (pool_ != NULL) {
		pool_->prev_ = pool;
	}
	pool_ = pool;

	freelist_ = (FreeNode*)((PoolNode*)pool + 1);

	char* fn = (char*)freelist_;

	for (int i=0 ; i < nodeCount_ ; ++i) {
		FreeNode* node = (FreeNode*)fn;
		node->parent_ = pool;

		if (i + 1 < nodeCount_) {
			FreeNode* next = (FreeNode*)(fn + nodeSize_);
			node->next_ = next;
			next->prev_ = node;
		}
		
		fn += nodeSize_;
	}
	poolCount_++;
	totalNode_ += nodeCount_;
}

void Allocator::FreePool(PoolNode* pool) {
	assert(pool->used_ == 0);
	char* fn = (char*)((PoolNode*)pool + 1);

	for (int i = 0; i < nodeCount_;i++) {
		FreeNode* node = (FreeNode*)fn;

		if (node == freelist_) {
			freelist_ = node->next_;
		}

		if (node->next_ != NULL) {
			node->next_->prev_ = node->prev_;
		}

		if (node->prev_ != NULL) {
			node->prev_->next_ = node->next_;
		}

		fn += nodeSize_;
	}

	if (pool_ == pool) {
		pool_ = pool->next_;
	}

	if (pool->next_ != NULL) {
		pool->next_->prev_ = pool->prev_;
	}

	if (pool->prev_ != NULL) {
		pool->prev_->next_ = pool->next_;
	}

	free(pool);

	poolCount_--;
	totalNode_ -= nodeCount_;
}

int Allocator::Dump() {
	return poolCount_ * poolSize_;
}
