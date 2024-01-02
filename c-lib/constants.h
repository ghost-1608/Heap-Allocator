/* +==============================================================+
 * | c-lib/constants.h -- Header containing quality of life       |
 * |                      constants                               |
 * |                                                              |
 * | (Heap-Allocator)                                             |
 * +==============================================================+
 * 
 * Contains constants for use by memory table and arena functions
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#define B 1
#define KiB 1024 * B
#define MiB 1024 * KiB

// Arena allocation size and minimum arena threshold space
#define __ARALLOC_SIZE 2 * KiB
#define __ARMIN_SIZE 8 * B

#endif
