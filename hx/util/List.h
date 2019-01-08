#ifndef LIST_H
#define LIST_H
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "PoolObject.h"

template<typename T>
struct ListNode final: public PoolObject<ListNode<T>> {
	T* data_;
	ListNode<T>* prev_;
	ListNode<T>* next_;

	static std::string GetClassName() {
		return std::string("ListNode");
	}
};

template<typename T>
struct List {
	typedef ListNode<T> Node;
	Node* head_;
	Node* tail_;
	int size_;

	List(): head_(NULL), tail_(NULL), size_(0) {
	}

	~List() {
		Node* cursor = head_;
		while(cursor) {
			Node* tmp = cursor;
			cursor = cursor->next_;
			delete tmp;
		}
	}

	bool Empty() {
		return head_ == NULL? true:false;
	}

	Node* Next(Node* node) {
		if (node == NULL)
			return head_;
		return head_->next_;
	}

	T* Front() {
		return head_->data_;
	}

	T* Back() {
		return tail_->data_;
	}

	void RemoveFront() {
		assert(head_ != NULL && tail_ != NULL);
		if (head_ == tail_) {
			delete head_;
			head_ = tail_ = NULL;
		} else {
			Node* node = head_;
			head_ = head_->next_;
			delete node;
		}
		size_--;
	}

	void RemoveBack() {
		assert(head_ != NULL && tail_ != NULL);
		if (head_ == tail_) {
			delete tail_;
			head_ = tail_ = NULL;
		} else {
			Node* node = tail_;
			tail_->prev_->next_ = NULL;
			tail_ = tail_->prev_;
			delete node;
		}
		size_--;
	}

	void PopHead(T*& obj) {
		obj = NULL;
		if (head_ == NULL)
			return;

		Node* node = NULL;
		if (head_ == tail_) {
			node = head_;
			head_ = tail_ = NULL;
			obj = node->data_;
		} else {
			node = head_;
			head_ = node->next_;
			obj = node->data_;
		}
		delete node;
		size_--;
	}

	void PopTail(T*& obj) {
		obj = NULL;
		if (tail_ == NULL)
			return;

		Node* node = NULL;
		if (head_ == tail_) {
			node = tail_;
			head_ = tail_ = NULL;
			obj = node->data_;
		} else {
			node = tail_;
			obj = node->data_;
			tail_->prev_->next_ = NULL;
			tail_ = tail_->prev_;
		}
		delete node;
		size_--;
	}

	void PushTail(T* obj) {
		Node* node = new Node();
		node->next_ = node->prev_ = NULL;
		node->data_ = obj;

		if (head_ == NULL) {
			head_ = tail_ = node;
		} else {
			tail_->next_ = node;
			node->prev_ = tail_;
			tail_ = node;
		}
		size_++;
	}

	void PushHead(T* obj) {
		Node* node = new Node();
		node->next_ = node->prev_ = NULL;
		node->data_ = obj;

		if (head_ == NULL) {
			head_ = tail_ = node;
		} else {
			node->next_ = head_;
			head_ = node;
		}
		size_++;
	}
};

#endif