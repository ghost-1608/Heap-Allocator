/* +==============================================================+
 * | c-lib/links.h -- Header containing external links            |
 * |                                                              |
 * | (Heap-Allocator)                                             |
 * +==============================================================+
 * 
 * Contains links to external functions:-
 *   - alloc()
 *   - dealloc()
 */

#include <stddef.h>

#ifndef LINKS_H
#define LINKS_H

extern void* alloc(size_t);
extern void dealloc(void*);

#endif
