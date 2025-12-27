#ifndef X_VM_H
#define X_VM_H

#include "xalloc.h"
#include "xvm_config.h"
#include "xstack.h"
#include "xobject.h"
#include "xtable.h"

typedef struct {
    xl_allocator* alloc;
    u8* ip;  // instruction ptr
    xl_stack* stack;
    xl_table strings;
    xl_obj* objects;
} xl_vm;

typedef enum {
    EXEC_OK,
    EXEC_COMPILE_ERROR,
    EXEC_RUNTIME_ERROR,
} xl_exec_result;

extern xl_vm vm;

void xl_vm_init(xl_allocator* alloc);
xl_exec_result xl_vm_exec(const char* source);

#endif
