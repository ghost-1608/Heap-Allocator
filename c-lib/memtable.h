/* +==============================================================+
 * | c-lib/memtable.h -- Memory table manager abstract header     |
 * |                                                              |
 * | (Heap-Allocator)                                             |
 * +==============================================================+
 * 
 * Contains function declerations and data types requierd to create
 * and manage the memory table
 * Mem table is used to keep track of all the allocations made
 * Two tables exist: one for free chunks, other for used chunks
 * Makes recycling memory allocation possible
 */

#include "links.h"

#ifndef MEMTABLE_H
#define MEMTABLE_H

// Memory arena related global variables
//   * Arena space availability counter
//   * Arena pointer
static size_t __aravailable = 0;
static void* __arpointer = NULL;

// Data types necessary:-
//   * Latest unique identifier
//   * Usage status code type
typedef uint64_t __UID_t;
typedef enum { MFREE, MUSED } __STAT_t;

// Struct used for mem table entry
struct __attribute__((packed)) __memtableEntry_struct
{
  __UID_t uid;
  size_t size;
  void* address;
  struct __memtableEntry_struct* plink;
  struct __memtableEntry_struct* nlink;
};
typedef struct __memtableEntry_struct __MTENTRY_t;

// Struct used for mem table
typedef struct
{
  __UID_t top;
  __MTENTRY_t* free;
  __MTENTRY_t* used;
} __MTABLE_t;

// Enum used for function return status codes
typedef enum
{
  __ARSZERR = -3,
  __ARERROR,
  __MTALERR,
  __MTSUCCESS,
  __MTFAILED
} __MTSTATC_t;

// Arena related function(s)
__MTSTATC_t __arensure(size_t);
// Memory table related functions
__MTSTATC_t __mtcreate(__MTABLE_t**);
__MTSTATC_t __mtadd(__MTABLE_t*, size_t, __STAT_t, void*);
__MTSTATC_t __mtmark(__MTABLE_t*, void*, __STAT_t);
void __mtinsert(__MTENTRY_t**, __MTENTRY_t*);

// void __mtsort(__MTABLE_t*); <-- Future implement?
#endif
