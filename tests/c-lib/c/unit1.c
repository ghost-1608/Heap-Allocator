//`{in: None, out: ["UID: 0 Size: 4 Address: 0xdeadbeef", "UID: 1 Size: 2 Address: 0xdeafbeed"]}`

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

  ret = errorlogger(__mtadd(mtable, 4, __MUSED, (void*) 0xdeadbeef));

  if (ret)
    return ret;

  row = mtable->used;

  printf("UID: %lu Size: %lu Address: %p\n", row->uid, row->size, row->address);

  ret = errorlogger(__mtadd(mtable, 2, __MUSED, (void*) 0xdeafbeed));

  if (ret)
    return ret;

  row = row->nlink;

  printf("UID: %lu Size: %lu Address: %p\n", row->uid, row->size, row->address);
  
  return 0;
}
