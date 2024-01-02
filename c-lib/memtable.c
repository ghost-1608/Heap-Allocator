#include "memtable.h"
#include "constants.h"

__MTSTATC_t __arensure(size_t size)
{
  if (size > __ARALLOC_SIZE)
    return __ARSZERR;
  
  if (__aravailable < size ||
      !__aravailable && !__arpointer ||
      __aravailable <= __ARMIN_SIZE)
  {
    __arpointer = alloc(__ARALLOC_SIZE);
    if (__arpointer <= (void*) 0)
      return __MTALERR;
    __aravailable = __ARALLOC_SIZE;
    return __MTSUCCESS;
  }

  if (!__aravailable || !__arpointer)
    return __ARERROR;
}

void __mtinsert(__MTENTRY_t** base_p, __MTENTRY_t* row)
{
  if (!row || !base_p)
    return;

  row->nlink = NULL;
  row->plink = NULL;

  if (!*base_p)
  {
    *base_p = row;
    return;
  }
  
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
      return;
    }
    else if (!cur->nlink)
    {
      cur->nlink = row;
      row->plink = cur;
      return;
    }
    prev = cur;
  }
}

__MTSTATC_t __mtcreate(__MTABLE_t** mtable_p)
{
  __MSTATC_t ret = __arensure(sizeof(__MTABLE_t));

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

__MTSTATC_t __mtadd(__MTABLE_t* mtable, size_t size, __STAT_t status, void* address)
{
  if (!mtable ||
      !size ||
      status != MFREE && status != MUSED ||
      !address)
    return __MTFAILED;

  __MSTATC_t ret = __arensure(sizeof(__MTABLE_t));

  switch (ret)
  {
    case __ARSZERR:
    case __ARERROR:
      return ret;
    case __MTSUCCESS:
  }
  
  __MTENTRY_t* row = __arpointer;
  __MTENTRY_t** base_p = NULL;

  __arpointer += sizeof(__MTENTRY_t);
  __aravailable -= sizeof(__MTENTRY_t);
  
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

__MTSTATC_t __mtmark(__MTABLE_t* mtable, void* address, __STAT_t status)
{
  __MTENTRY_t** base_p = NULL;

  switch (status)
  {
    case MFREE:
      base_p = &mtable->used;
      break;
    case MUSED:
      base_p = &mtable->free;
  }

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
