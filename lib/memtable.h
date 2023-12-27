#include "types.h"
#include "links.h"

#ifndef MEMTABLE_H
#define MEMTABLE_H

static __HAbool_t isarena = false;
static void* a_pointer;

struct __attribute__((packed)) __MTENTRY_t
{
  __UID_t uid;
  size_t size;
  void* address;
  __MTENTRY_t* plink;
  __MTENTRY_t* nlink;
};

typedef struct __MTENTRY_t __MTENTRY_t;

typedef struct
{
  __UID_t top;
  __MTENTRY_t* free;
  __MTENTRY_t* used;
} __MTABLE_t;

typedef enum
{
  __MTSUCCESS,
  __MTFAILED
} __HASTATC_t;

__HASTATC_t __mtadd(__MTABLE_t*, size_t, __STAT_t, void*);
__HASTATC_t __mtmark(__MTABLE_t*, __UID_t, __STAT_t);
void __mtinsert(__MTENTRY_t**, __MTENTRY_t*);

// void __mtsort(__MTABLE_t*);
#endif
