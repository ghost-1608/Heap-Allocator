#include "memtable.h"

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

__MTABLE_t* __mtable = NULL;

void* halloc(size_t);
int mfree(void*);

size_t __chunkdssize(void* address);

#endif
