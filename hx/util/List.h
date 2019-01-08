#ifndef LIST_H
#define LIST_H
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "PoolObject.h"

template<typename T>
struct ListNode final: public PoolObject< ListNode<T> >
{
	T* _data;
	ListNode<T>* _prev;
	ListNode<T>* _next;
};

template<typename T>
struct List
{
	typedef ListNode<T> Node;
	Node* _head;
	Node* _tail;
	int _size;

	List():
		_head(NULL),
		_tail(NULL),
		_size(0)
	{
	}

	bool Empty()
	{
		return _head == NULL? true:false;
	}

	Node* Next(Node* node)
	{
		if (node == NULL)
			return _head;
		return _head->_next;
	}

	T* Front()
	{
		return _head->_data;
	}

	T* Back()
	{
		return _tail->_data;
	}

	void RemoveFront()
	{
		assert(_head != NULL && _tail != NULL);
		if (_head == _tail)
		{
			delete _head;
			_head = _tail = NULL;
		}
		else
		{
			Node* node = _head;
			_head = _head->_next;
			delete node;
		}
		_size--;
	}

	void RemoveBack()
	{
		assert(_head != NULL && _tail != NULL);
		if (_head == _tail)
		{
			delete _tail;
			_head = _tail = NULL;
		}
		else
		{
			Node* node = _tail;
			_tail->_prev->_next = NULL;
			_tail = _tail->_prev;
			delete node;
		}
		_size--;
	}

	void PopHead(T*& obj)
	{
		obj = NULL;
		if (_head == NULL)
			return;

		Node* node = NULL;
		if (_head == _tail)
		{
			node = _head;
			_head = _tail = NULL;
			obj = node->_data;
		}
		else
		{
			node = _head;
			_head = node->_next;
			obj = node->_data;
		}
		delete node;
		_size--;
	}

	void PopTail(T*& obj)
	{
		obj = NULL;
		if (_tail == NULL)
			return;

		Node* node = NULL;
		if (_head == _tail)
		{
			node = _tail;
			_head = _tail = NULL;
			obj = node->_data;
		}
		else
		{
			node = _tail;
			obj = node->_data;
			_tail->_prev->_next = NULL;
			_tail = _tail->_prev;
		}
		delete node;
		_size--;
	}

	void PushTail(T* obj)
	{
		Node* node = new Node();
		node->_next = node->_prev = NULL;
		node->_data = obj;

		if (_head == NULL)
			_head = _tail = node;
		else
		{
			_tail->_next = node;
			node->_prev = _tail;
			_tail = node;
		}
		_size++;
	}

	void PushHead(T* obj)
	{
		Node* node = new Node();
		node->_next = node->_prev = NULL;
		node->_data = obj;

		if (_head == NULL)
			_head = _tail = node;
		else
		{
			node->_next = _head;
			_head = node;
		}
		_size++;
	}
};

#endif