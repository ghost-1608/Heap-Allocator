/* +==============================================================+
 * | c-lib/allocator.c -- Module containing user functions used   |
 * |                      for managing allocations                |
 * |                                                              |
 * | (Heap-Allocator)                                             |
 * +==============================================================+
 * 
 * Contains user-accessible functions to create and free memory
 * allocations
 * Two user functions exist:
 *  - alloc(): To make memory allocations
 *  - mfree(): To free previously allocated memory
 */

#include "allocator.h"

/*
 * User-accessible function used to allocate memory specified
 * by size
 * Buffer paddings of size MBUFFER_PADDING added on both sides
 * inside allocation
 * Each allocation kept track of using a memory table
 * Previously freed allocations can be re-allocated if the size
 * requests match
 * For re-allocation, best fit is searched
 */
void* halloc(size_t size)
{
  // Guard for zero size
  if (!size)
    return NULL;

  // Create memory table if it doesn't exist
  if (!__mtable)
    __mtcreate(&__mtable);

  // Declare memtable entry to be used to track the allocation
  __MTENTRY_t* entry = NULL;

  // Redefine size by adding symmetric padding
  size += 2 * MBUFFER_PADDING;
  
  // If previously freed allocations exist, search for best fit
  // for current request
  if (__mtable->free)
    entry = __mtsearch(__mtable, __MFREE, size, NULL);

  // If entry found from previous search, mark as used, return
  // its address
  // (NOTE) This if-statement was to guard against a failed search 
  //        result. The decision to add it outside the previous
  //        if-statment was to prevent a pyramid of doom.
  if (entry)
  {
    __mtmark(__mtable, entry, __MUSED, 0);
    return (void*) entry->address;
  }
  
  // Following section is to make alloc() calls to handle the case
  // where a fitting previously freed allocation wasn't found
 
  // Make alloc() call with requested size and store address
  void *address = alloc(size);

  // Error handling for failed alloc call (returns NULL)
  if (address <= (void*) 0)
    return NULL;

  // Add newly created allocation to memory table (used table), with
  // symmetric padding
  __mtadd(__mtable, size, __MUSED, address, address + MBUFFER_PADDING);
  
  // Return address properly padded with MBUFFER_PADDING
  return (void*) address + MBUFFER_PADDING;
}

/*
 * User-accessible function used to free previously allocated memory
 * Takes address of allocation (as returned by halloc() call)
 * Changes the entry table of the allocation entry in memtable
 * 
 * (NOTE) mfree() does not call dealloc() even though memory
 *        allocations are implemented using alloc() calls. This is done
 *        since dealloc() simply changes a flag byte in the allocation
 *        chunk header. And moreover, dealloc() is an extremely tiny
 *        function, calling which would only increase execution overhead
 *        without any benefits. The function, however, is linked in this
 *        library.
 */
int mfree(void* address)
{
  // Guard against null address
  if (!address)
    return 1;

  // Checks if allocations exist
  if (!__mtable->used)
    return 1;

  // Search for alloction by address
  __MTENTRY_t* entry = __mtsearch(__mtable, __MUSED, 0, address);

  // Exit if allocation not found
  if (!entry)
    return 1;

  // Move entry to free table
  __mtmark(__mtable, entry, __MFREE, 0);

  return 0;
}
