#include "xvm.h"

/*
 * GLOBAL VM instance
 */
xl_vm vm;

void xl_vm_init(xl_allocator* alloc) {
    assert(alloc);
    vm.alloc = alloc;
}

xl_exec_result xl_vm_exec(const char* source) {
    return EXEC_OK;
}
