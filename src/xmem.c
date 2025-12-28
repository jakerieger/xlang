#include "xmem.h"

void xl_vm_mem_init(xl_vm_mem* mem, size_t size_perm, size_t size_gen, size_t size_temp) {
    mem->permanent  = xl_alloc_create(size_perm);
    mem->generation = xl_alloc_create(size_gen);
    mem->temporary  = xl_alloc_create(size_temp);
}

void xl_vm_mem_destroy(xl_vm_mem* mem) {
    xl_alloc_destroy(mem->permanent);
    xl_alloc_destroy(mem->generation);
    xl_alloc_destroy(mem->temporary);
}
