#include "types.h"
#include "links.h"

#ifndef MEMTABLE_H
#define MEMTABLE_H

static __HAbool_t isarena = FALSE;
static void* arena_p;

struct __attribute__((packed)) __memtableEntry_struct
{
  __UID_t uid;
  size_t size;
  void* address;
  struct __memtableEntry_struct* plink;
  struct __memtableEntry_struct* nlink;
};

typedef struct __memtableEntry_struct __MTENTRY_t;

typedef struct
{
  __UID_t top;
  __MTENTRY_t* free;
  __MTENTRY_t* used;
} __MTABLE_t;

typedef enum
{
  __MTALERR = -1,
  __MTSUCCESS,
  __MTFAILED
} __MTSTATC_t;

__MTSTATC_t __mtcreate(__MTABLE_t**);
__MTSTATC_t __mtadd(__MTABLE_t*, size_t, __STAT_t, void*);
__MTSTATC_t __mtmark(__MTABLE_t*, __UID_t, __STAT_t);
void __mtinsert(__MTENTRY_t**, __MTENTRY_t*);

// void __mtsort(__MTABLE_t*);
#endif