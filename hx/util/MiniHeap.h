#ifndef MINHEAP_H
#define MINHEAP_H

#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>



struct element 
{
	int index;
};

struct minheap
{
	int cap;
	int size;
	void* ud;
	int (*less)(void* ud,struct element *l,struct element *r);
	struct element **elts;
};


struct minheap * minheap_new(int cap,void* ud,int (*less)(void* ud,struct element *l,struct element *r));
void minheap_delete(struct minheap *mh);
void minheap_clear(struct minheap * mh,void (*clear)(struct element *elt));
void minheap_push(struct minheap * mh,struct element * elt);
struct element * minheap_pop(struct minheap * mh);

#define MINHEAP_TOP(mh) (mh->size > 0 ? mh->elts[1]:NULL)

#endif