#include "xmem.h"

void xl_vm_mem_init(xl_vm_mem* mem) {
    mem->permanent  = xl_alloc_create(XL_MB(4));
    mem->generation = xl_alloc_create(XL_MB(1));
    mem->temporary  = xl_alloc_create(XL_KB(1));
}

void xl_vm_mem_destroy(xl_vm_mem* mem) {
    xl_alloc_destroy(mem->permanent);
    xl_alloc_destroy(mem->generation);
    xl_alloc_destroy(mem->temporary);
}
