/* +==============================================================+
 * | c-lib/allocator.h -- Module containing declaration for user  |
 * |                      functions used for managing allocations |
 * |                                                              |
 * | (Heap-Allocator)                                             |
 * +==============================================================+
 * 
 * Contains decleration of user-accessible functions to create and
 * free memory allocations
 * Two user functions exist:
 *  - alloc(): To make memory allocations
 *  - mfree(): To free previously allocated memory
 */

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "memtable.h"

// Macros changing allocation behaviour
#define MBUFFER_PADDING 16
#define ALLOC_HEADER_SIZE 0x10

// Global variable used to store the memtable
static __MTABLE_t* __mtable = NULL;

// Function to allocate memory on the heap
void* halloc(size_t);
// Function to free previously allocated memory on the heap
int mfree(void*);

#endif
