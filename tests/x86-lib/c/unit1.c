//`{in: None, out: "Hello, World!"}`

// Unit test - 1
// =============
//    * Allocates storage to store string (14 byte)
//    * Stores heap buffer address
//    * Copies string "Hello, World!" to the buffer
//    * Uses printf() to print the string (from buffer)
//    * De-allocates memory

#include <stdio.h>
#include <string.h>

// Link the assembly functions
extern void* alloc(size_t size);
extern void dealloc(void* address);

int main()
{
  // Call alloc to allocate sufficient size
  void* buffer = alloc(sizeof(char) * 14);

  // Copy the string to the buffer
  strcpy(buffer, "Hello, World!");

  // Print the string
  printf("%s\n", (char*) buffer);

  // Deallocate buffer
  dealloc(buffer);

  // Exit
  return 0;
}
