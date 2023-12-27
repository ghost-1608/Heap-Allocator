#include "memtable.h"
#include "constants.h"

void __mtinsert(__MTENTRY_t** segment, __MTENTRY_t* row)
{
  for (__MTENTRY_t* cur = *segment, prev = NULL; cur; cur = cur->nlink)
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
	*segment = row;
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
}

__HASTATC_t __mtadd(__MTABLE_t* mtable, size_t size, __STAT_t status, void* address)
{
  if (!isarena)
  {
    arena_p = alloc(MTARENA_SIZE);
    mtable->top = 0;
    mtable->free = NULL;
    mtable->used = NULL;
    isarena = TRUE;
  }
  
  __MTENTRY_t* row = arena_p;
  __MTENTRY_t** segment = NULL;

  arena_p += sizeof(__MTENTRY_t);
  
  row->uid = ++mtable->top;
  row->size = size;
  row->address = address;

  switch (status)
  {
    case MFREE:
      segment = &mtable->free;
      break;
    case MUSED:
      segment = &mtable->used;
  }

  __mtinsert(segment, row);

  if (!*table)
  {
    *segment = row;
    row->nlink = NULL;
    row->plink = NULL;
  }
}

__HASTATC_t __mtmark(__MTABLE_t* mtable, __UID_t uid, __STAT_t status)
{
  __MTABLE_t** segment = NULL;

  switch (status)
  {
    case MFREE:
      table = &mtable->used;
      break;
    case MUSED:
      table = &mtable->free;
  }

  for (__MTENTRY_t* cur = *segment; cur; cur = cur->nlink)
  {
    if (cur->uid == uid)
    {
      if (cur->plink)
	cur->plink->nlink = cur->nlink;
      if (cur->nlink)
	cur->nlink->plink = cur->plink;
      __mtinsert(segment, cur);
    }
  }
}
