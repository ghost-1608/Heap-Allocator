//`{in: None, out: "The number is 5"}`

#include "../../../c-lib/allocator.h"
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
  int* num;

  num = (int*) halloc(sizeof(int));

  if (!num)
  {
    perror("Allocation error!");
    return 1;
  }

  *num = 5;

  printf("The number is %d\n", *num);
  
  return 0;
}
