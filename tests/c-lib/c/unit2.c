//`{in: None, out: ["Size: 2 Chunk Address: 0xdeafbeed Address: 0xdeafbeed", "Size: 4 Chunk Address: 0xdeadbeef Address: 0xdeadbeef"]}`

#include "../../../c-lib/memtable.h"
#include <stdio.h>

int errorlogger(__MTSTATC_t code)
{
  switch (code)
  {
    case __ARSZERR:
      perror("Arena size error");
      return 1;
    case __ARERROR:
      perror("Arena other error");
      return 2;
    case __MTALERR:
      perror("Mem table allocation error");
      return 3;
    case __MTFAILED:
      perror("Mem table error");
      return 4;
    case __MTSUCCESS:
  }
  return 0;
}

int main()
{
  __MTABLE_t* mtable;
  __MTENTRY_t* row;

  int ret;
  
  ret = errorlogger(__mtcreate(&mtable));

  if (ret)
    return ret;

  ret = errorlogger(__mtadd(mtable, 4, __MUSED, (void*) 0xdeadbeef, (void*) 0xdeadbeef));

  if (ret)
    return ret;

  ret = errorlogger(__mtadd(mtable, 2, __MUSED, (void*) 0xdeafbeed, (void*) 0xdeafbeed));

  if (ret)
    return ret;

  ret = errorlogger(__mtmark(mtable, mtable->used, __MFREE));

  if (ret)
    return ret;

  row = mtable->used;

  printf("Size: %lu Chunk Address: %p Address: %p\n", row->size, row->chunkaddr, row->address);

  row = mtable->free;

  printf("Size: %lu Chunk Address: %p Address: %p\n", row->size, row->chunkaddr, row->address);  
  
  return 0;
}
