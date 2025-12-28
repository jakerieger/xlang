#ifndef X_MEM_H
#define X_MEM_H

#include "xalloc.h"

typedef struct {
    xl_allocator* permanent;   // GC roots, global state
    xl_allocator* generation;  // Current execution generation
    xl_allocator* temporary;   // Expression evaluation, stack frames
} xl_vm_mem;

void xl_vm_mem_init(xl_vm_mem* mem, size_t size_perm, size_t size_gen, size_t size_temp);
void xl_vm_mem_destroy(xl_vm_mem* mem);

#endif
