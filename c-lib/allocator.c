#include "allocator.h"

size_t __chunkdssize(void* address)
{
  if (!address)
    return 0;
  
  size_t size = * (size_t*) (address - 0x18);

  return size >> 8;
}

void* halloc(size_t size)
{
  if (!size)
  {
    return NULL;
  }

  if (!__mtable)
  {
    __mtcreate(&__mtable);
  }

  if (__mtable->free)
  {
    __MTENTRY_t* entry = __mtsearch(__mtable, __MFREE, size, NULL);
    __mtmark(__mtable, entry, __MUSED);
    return entry->address;
  }

  void* address = NULL;
  address = alloc(size);

  if (address <= (void*) 0)
    return NULL;

  size = __chunkdssize(address);

  __mtadd(__mtable, size, __MUSED, address, address);

  return address;
}

int mfree(void* address)
{
  if (!address)
    return -1;

  if (!__mtable->used)
    return -1;

  __MTENTRY_t* entry = __mtsearch(__mtable, __MUSED, 0, address);
  __mtmark(__mtable, row, __MFREE);

  return 0;
}
