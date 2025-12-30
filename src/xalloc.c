#include "xalloc.h"

xl_allocator* xl_alloc_create(u64 capacity) {
    xl_allocator* out = (xl_allocator*)malloc(capacity);
    if (!out) {
        xl_panic(XL_ERR_ALLOCATION_FAILED, "failed to allocate memory for xl_allocator");
    }

    out->pos = XL_ALLOC_POS;
    out->cap = capacity;

    return out;
}

void xl_alloc_destroy(xl_allocator* alloc) {
    free(alloc);
}

void* xl_alloc_push(xl_allocator* alloc, u64 size, bool non_zero) {
    u64 pos_aligned = XL_ALIGN_UP(alloc->pos, XL_PAGESIZE);
    u64 new_pos     = pos_aligned + size;

    if (new_pos > alloc->cap) {
        xl_panic(XL_ERR_ALLOCATION_FAILED,
                 "attempted to allocate unavailable memory (capacity: %lu bytes, attempted: %lu bytes)",
                 alloc->cap,
                 new_pos);
    }

    alloc->pos = new_pos;
    u8* out    = (u8*)alloc + pos_aligned;

    if (!non_zero) {
        memset(out, 0, size);
    }

    return out;
}

void xl_alloc_pop(xl_allocator* alloc, u64 size) {
    size = XL_MIN(size, alloc->pos - XL_ALLOC_POS);
    alloc->pos -= size;
}

void xl_alloc_pop_to(xl_allocator* alloc, u64 pos) {
    u64 size = pos < alloc->pos ? alloc->pos - 1 : 0;
    xl_alloc_pop(alloc, size);
}

void xl_alloc_clear(xl_allocator* alloc) {
    xl_alloc_pop_to(alloc, XL_ALLOC_POS);
}
