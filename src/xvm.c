#include "xvm.h"
#include "xalloc.h"
#include "xmem.h"
#include <assert.h>

/*
 * GLOBAL VM instance
 */
xl_vm vm;

void xl_vm_init(xl_vm_config config) {
    xl_vm_mem_init(&vm.mem, config.mem_size_permanent, config.mem_size_generation, config.mem_size_temporary);
}

xl_exec_result xl_vm_exec(const char* source) {
    return EXEC_COMPILE_ERROR;
}

void xl_vm_shutdown() {
    xl_vm_mem_destroy(&vm.mem);
}
