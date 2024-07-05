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
#ifndef LINKS_H
#define LINKS_H

#include <stddef.h>

extern void* alloc(size_t);
extern void  dealloc(void*);

#endif
