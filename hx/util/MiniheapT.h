#ifndef MINIHEAPT_H
#define MINIHEAPT_H
#include "MiniHeap.h"
#include "PoolObject.h"

template<class T>
class MiniheapT;

template<class T>
struct MiniHeapNodeT : public element, public PoolObject< MiniHeapNodeT<T> >
{
	MiniHeapNodeT(T* data)
	{
		_data = data;
	}
	T* _data;

	static std::string GetClassName() {
		return std::string("MiniHeapNodeT");
	}
};

template<class T>
class MiniheapT
{
public:

public:
	MiniheapT(int size = 64)
	{
		_heap = minheap_new(size,this,MiniheapT<T>::Compare);
	}

	virtual ~MiniheapT()
	{
		minheap_delete(_heap);
	}

	void Push(T* obj)
	{
		MiniHeapNodeT<T>* node = new MiniHeapNodeT<T>(obj);
		node->index = 0;
		minheap_push(_heap,node);
	}

	void Pop(T*& obj)
	{
		obj = NULL;
		MiniHeapNodeT<T>* top = (MiniHeapNodeT<T>*)MINHEAP_TOP(_heap);
		if (top == NULL)
			return;
		
		if (this->CanPop(top->_data))
		{
			MiniHeapNodeT<T>* elt = (MiniHeapNodeT<T>*)minheap_pop(_heap);
			obj = elt->_data;
			delete elt;
		}
	}

	static int Compare(void* ud,element* left,element* right)
	{
		MiniHeapNodeT<T>* l = (MiniHeapNodeT<T>*)left;
		MiniHeapNodeT<T>* r = (MiniHeapNodeT<T>*)right;
		MiniheapT<T>* self = (MiniheapT<T>*)ud;
		return self->DoCompare(l->_data,r->_data);
	}

	virtual int DoCompare(T* left,T* right) = 0;

	virtual int CanPop(T* obj) = 0;
private:
	minheap* _heap;
};

#endif