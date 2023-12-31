#include "memtable.h"
#include "constants.h"

void __mtinsert(__MTENTRY_t** base_p, __MTENTRY_t* row)
{
  for (__MTENTRY_t *cur = *base_p, *prev = NULL; cur; cur = cur->nlink)
  {
    if (cur->size <= row->size)
    {
      if (cur->plink)
      {
	cur->plink = row;
	prev->nlink = row;
      }
      else
      {
	cur->plink = row;
	*base_p = row;
      }
	
      row->plink = prev;
      row->nlink = cur;
    }
    else
    {
      row->nlink = cur;
      row->plink = NULL;
      cur->plink = row;
      break;
    }
    prev = cur;
  }

  if (!*base_p)
  {
    *base_p = row;
    row->nlink = NULL;
    row->plink = NULL;
  }
}

__MTSTATC_t __mtcreate(__MTABLE_t** mtable_p)
{
  if (!arena_p)
  {
    arena_p = alloc(MTARENA_SIZE);
    if (!arena_p)
      return __MTALERR;
  }

  *mtable_p = (__MTABLE_t*) arena_p;
  arena_p += sizeof(__MTABLE_t);

  (*mtable_p)->top = 0;
  (*mtable_p)->free = NULL;
  (*mtable_p)->used = NULL;

  return __MTSUCCESS;
}

__MTSTATC_t __mtadd(__MTABLE_t* mtable, size_t size, __STAT_t status, void* address)
{
  if (!mtable ||
      !size ||
      status != MFREE && status != MUSED ||
      !address)
    return __MTFAILED;
  
  __MTENTRY_t* row = arena_p;
  __MTENTRY_t** base_p = NULL;

  arena_p += sizeof(__MTENTRY_t);
  
  row->uid = ++mtable->top;
  row->size = size;
  row->address = address;

  switch (status)
  {
    case MFREE:
      base_p = &mtable->free;
      break;
    case MUSED:
      base_p = &mtable->used;
  }

  __mtinsert(base_p, row);

  return __MTSUCCESS;
}

__MTSTATC_t __mtmark(__MTABLE_t* mtable, __UID_t uid, __STAT_t status)
{
  __MTENTRY_t** base = NULL;

  switch (status)
  {
    case MFREE:
      base = &mtable->used;
      break;
    case MUSED:
      base = &mtable->free;
  }

  for (__MTENTRY_t* cur = *base; cur; cur = cur->nlink)
  {
    if (cur->uid == uid)
    {
      if (cur->plink)
	cur->plink->nlink = cur->nlink;
      if (cur->nlink)
	cur->nlink->plink = cur->plink;
      __mtinsert(base, cur);
    }
  }
}
