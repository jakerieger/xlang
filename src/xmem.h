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

typedef struct xl_obj xl_obj;

void* xl_mem_realloc(void* ptr, size_t old_size, size_t new_size);
void xl_mem_free_objects();
static void xl_mem_free_object(xl_obj* obj);

#define XL_ALLOCATE(type, count) (type*)xl_mem_realloc(NULL, 0, sizeof(type) * (count))
#define XL_GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)
#define XL_GROW_ARRAY(type, pointer, old_count, new_count)                                                             \
    (type*)xl_mem_realloc(pointer, sizeof(type) * (old_count), sizeof(type) * (new_count))
#define XL_FREE_ARRAY(type, pointer, old_count) xl_mem_realloc(pointer, sizeof(type) * (old_count), 0)
#define XL_FREE(type, pointer) xl_mem_realloc(pointer, sizeof(type), 0)

#endif
