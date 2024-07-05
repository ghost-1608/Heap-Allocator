/* +==============================================================+
 * | c-lib/memtable.c -- Memory table manager source code         |
 * |                                                              |
 * | (Heap-Allocator)                                             |
 * +==============================================================+
 * 
 * Contains functions required to create and manage the memory table
 * Mem table is used to keep track of all the allocations made
 * Two tables exist: one for free chunks, other for used chunks
 * Makes recycling memory allocation possible
 */

#include "memtable.h"
#include "constants.h"

/*
 * Ensures arena has enough space to operate
 * Allocates space for new arena in case not enough /
 * no arena exists
 * Input: size to check
 * Output: Status code
 */
__MTSTATC_t __arensure(size_t size)
{
  // Error check for invalid arena allocation size /
  // invalid request size
  if (size > __ARALLOC_SIZE)
    return __ARSZERR;

  // Make call to alloc() if:-
  //   * 'size' is provided and available size is lesser
  //   * Arena hasn't been created
  //   * Available space in arena is below min threshold
  if (__aravailable < size ||
      __aravailable == 0 && __arpointer == NULL ||
      __aravailable <= __ARMIN_SIZE)
  {
    // Try to allocate space for arena
    void* tmp = alloc(__ARALLOC_SIZE);

    // Error check for buffer returned by alloc()
    if (tmp <= (void*) 0)
      return __MTALERR;

    // Set arena pointer to allocated address
    __arpointer = tmp;
    // Set available size counter
    __aravailable = __ARALLOC_SIZE;
  }

  return __MTSUCCESS;
}

/*
 * Function to insert new row entry to mem table
 * Doubly-linked list non-decreasing order entry based on
 * size
 * Input: Table base pointer address, new row
 * Output: Status code
 */
void __mtinsert(__MTENTRY_t** base_p, __MTENTRY_t* row)
{
  // Error check for invalid entry row and table base
  // pointer address values
  if (row == NULL || base_p == NULL)
    return;

  // Init next and prev link values
  row->nlink = NULL;
  row->plink = NULL;

  // In case table has no entry
  if (*base_p == NULL)
  {
    *base_p = row;
    return;
  }

  // Loop through link list to find appropriate position of insertion
  // (non-decreasing order by allocation size)
  for (__MTENTRY_t *cur = *base_p; cur; cur = cur->nlink)
  {
    if (cur->size >= row->size)
    {
      // Current element has lesser size (insert new element before)
      if (cur->plink)
        // Current element isn't first element
        cur->plink->nlink = row;
      else
        // Current element is first element
        *base_p = row;
      row->plink = cur->plink;
      cur->plink = row;
      row->nlink = cur;
      return;
    }
    else if (!cur->nlink)
    {
      // Current element is last element (insert new element after)
      // (no size checks required further)
      cur->nlink = row;
      row->plink = cur;
      return;
    }
  }
}

/*
 * Creates new mem table
 * Input: Pointer to mem table
 * Output: Status code
 */
__MTSTATC_t __mtcreate(__MTABLE_t** mtable_p)
{
  __MTSTATC_t ret = __arensure(sizeof(__MTABLE_t));

  switch (ret)
  {
    case __ARSZERR:
    case __ARERROR:
      return ret;
    case __MTSUCCESS:
  }
  
  *mtable_p = (__MTABLE_t*) __arpointer;

  __arpointer += sizeof(__MTABLE_t);
  __aravailable -= sizeof(__MTABLE_t);

  (*mtable_p)->free = NULL;
  (*mtable_p)->used = NULL;

  return __MTSUCCESS;
}

/*
 * Adds new entry to mem table
 * Error checks and minor work done
 * Input: Mem table, size of allocation, usage status, buffer address
 * Output: Status code
 */
__MTSTATC_t __mtadd(__MTABLE_t* mtable, size_t size, __STAT_t status, void* chunk_address, void* address)
{
  // Error check for invalid arguments
  if (!mtable ||
      !size ||
      status != __MFREE && status != __MUSED ||
      !chunk_address || 
      !address)
    return __MTFAILED;

  __MTSTATC_t ret = __arensure(sizeof(__MTABLE_t));

  // Forward arena status codes
  switch (ret)
  {
    case __ARSZERR:
    case __ARERROR:
      return ret;
    case __MTSUCCESS:
  }
  
  __MTENTRY_t* row = (__MTENTRY_t*) __arpointer;
  __MTENTRY_t** base_p = NULL;

  // Increment arena pointer
  // Decrement available size counter
  __arpointer += sizeof(__MTENTRY_t);
  __aravailable -= sizeof(__MTENTRY_t);

  // Set values to entry
  row->size = size;
  row->chunkaddr = chunk_address;
  row->address = address;

  // Select table by usage status
  switch (status)
  {
    case __MFREE:
      base_p = &mtable->free;
      break;
    case __MUSED:
      base_p = &mtable->used;
  }

  // Insert as linked list
  __mtinsert(base_p, row);
 
  return __MTSUCCESS;
}

/*
 * Marks existing entry under a different usage status
 * Input: Mem table, buffer address, usage status, size
 * Output: Status code
 */
__MTSTATC_t __mtmark(__MTABLE_t* mtable, __MTENTRY_t* cur, __STAT_t status, size_t size)
{
  // Error check for invalid arguments
  if (!mtable || 
      !cur || 
      status != __MFREE && status != __MUSED)
    return __MTFAILED;
  
  __MTENTRY_t **curbase_p = NULL, **newbase_p = NULL;

  // Select which table to search based on usage status argument
  switch (status)
  {
    case __MFREE:
      curbase_p = &mtable->used;
      newbase_p = &mtable->free;
      break;
    case __MUSED:
      curbase_p = &mtable->free;
      newbase_p = &mtable->used;
  }

  // Change required values and add the entry to the requested table
  if (cur->plink)
    cur->plink->nlink = cur->nlink;
  else
    *curbase_p = cur->nlink;
  if (cur->nlink)
    cur->nlink->plink = cur->plink;
  __mtinsert(newbase_p, cur);

  // Set size to new size if provided
  if (size)
    cur->size = size;
  
  return __MTSUCCESS;
}

/*
 * Searches for an entry in the mem table by either size or address
 * Input: Mem table, usage status, allocation size, buffer address
 * Output: Row entry address
 */
__MTENTRY_t* __mtsearch(__MTABLE_t* mtable, __STAT_t status, size_t size, void* address)
{
  // Error checks for invalid arguments
  if (!mtable ||
      status != __MFREE && status != __MUSED ||
      !size && !address)
    return NULL;

  __MTENTRY_t **base_p, *cur = NULL;

  // Selecting correct base table address
  switch (status)
  {
    case __MFREE:
      base_p = &mtable->free;
      break;
    case __MUSED:
      base_p = &mtable->used;
  }

  // Search by size or address
  for (cur = *base_p; cur; cur = cur->nlink)
  {
    if (size && cur->size >= size ||
        address && cur->address == address)
      break;
  }

  return cur;
}
