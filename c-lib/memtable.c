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
      !__aravailable && !__arpointer ||
      __aravailable <= __ARMIN_SIZE)
  {
    // Set arena pointer
    __arpointer = alloc(__ARALLOC_SIZE);

    // Error check for buffer returned by alloc()
    if (__arpointer <= (void*) 0)
      return __MTALERR;

    // Set available size counter
    __aravailable = __ARALLOC_SIZE;

    return __MTSUCCESS;
  }

  // Error check for invalid arena pointer and availability
  // counter values
  if (!__aravailable || !__arpointer)
    return __ARERROR;
}

/*
 * Function to insert new row entry to mem table
 * Doubly-linked list decending order entry
 * Input: Table base pointer address, new row
 * Output: Status code
 */
void __mtinsert(__MTENTRY_t** base_p, __MTENTRY_t* row)
{
  // Error check for invalid entry row and table base
  // pointer address values
  if (!row || !base_p)
    return;

  // Init next and prev link values
  row->nlink = NULL;
  row->plink = NULL;

  // In case table has no entry
  if (!*base_p)
  {
    *base_p = row;
    return;
  }

  // Loop through link list to find appropriate position of insertion
  // (descending order by allocation size)
  for (__MTENTRY_t *cur = *base_p, *prev = NULL; cur; cur = cur->nlink)
  {
    if (cur->size <= row->size)
    {
      // Current element has lesser size (insert new element before)
      if (cur->plink)
      {
	// Current element isn't first element
	cur->plink = row;
	prev->nlink = row;
      }
      else
      {
	// Current element is first element
	cur->plink = row;
	*base_p = row;
      }
      row->plink = prev;
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
    prev = cur;
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

  (*mtable_p)->top = 0;
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
__MTSTATC_t __mtadd(__MTABLE_t* mtable, size_t size, __STAT_t status, void* address)
{
  // Error check for invalid arguments
  if (!mtable ||
      !size ||
      status != MFREE && status != MUSED ||
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
  row->uid = ++mtable->top;
  row->size = size;
  row->address = address;

  // Select table by usage status
  switch (status)
  {
    case MFREE:
      base_p = &mtable->free;
      break;
    case MUSED:
      base_p = &mtable->used;
  }

  // Insert as linked list
  __mtinsert(base_p, row);

  return __MTSUCCESS;
}

/*
 * Marks existing entry under a different usage status
 * Input: Mem table, buffer address, usage status
 * Output: Status code
 */
__MTSTATC_t __mtmark(__MTABLE_t* mtable, void* address, __STAT_t status)
{
  __MTENTRY_t** base_p = NULL;

  // Select which table to search based on usage status argument
  switch (status)
  {
    case MFREE:
      base_p = &mtable->used;
      break;
    case MUSED:
      base_p = &mtable->free;
  }

  // Search appropriate table only,
  // change required values and add the entry to the requested table
  for (__MTENTRY_t* cur = *base_p; cur; cur = cur->nlink)
  {
    if (cur->address == address)
    {
      if (cur->plink)
	cur->plink->nlink = cur->nlink;
      if (cur->nlink)
	cur->nlink->plink = cur->plink;
      __mtinsert(base_p, cur);
    }
  }
}
