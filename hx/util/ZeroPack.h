#ifndef ZEROPACKER_H
#define ZEROPACKER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int Zeropack(void * srcv, int srcsz, void * bufferv, int bufsz);
int Zerounpack(void * srcv, int srcsz, void* bufferv,int* size);

#endif