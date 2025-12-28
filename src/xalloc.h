#ifndef X_ALLOC_H
#define X_ALLOC_H

#include "xcommon.h"

/*
 * Implements a simple arena alloctor for general-purpose
 * allocations.
 */

#define XL_ALLOC_POS (sizeof(xl_allocator))

typedef struct {
    u64 cap;
    u64 pos;
} xl_allocator;

xl_allocator* xl_alloc_create(u64 capacity);
void xl_alloc_destroy(xl_allocator* alloc);
void* xl_alloc_push(xl_allocator* alloc, u64 size, bool non_zero);
void xl_alloc_pop(xl_allocator* alloc, u64 size);
void xl_alloc_pop_to(xl_allocator* alloc, u64 pos);
void xl_alloc_clear(xl_allocator* alloc);

#endif
