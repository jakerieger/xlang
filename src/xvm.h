#ifndef X_VM_H
#define X_VM_H

#include "xalloc.h"
#include "xvm_config.h"
#include "xstack.h"
#include "xobject.h"
#include "xtable.h"
#include "xmem.h"
#include "xchunk.h"

typedef struct {
    xl_vm_mem mem;
    xl_chunk* chunk;
    u8* ip;  // instruction ptr
    xl_stack stack;
    xl_table strings;
    xl_table globals;
    xl_obj* objects;
} xl_vm;

typedef enum {
    EXEC_OK,
    EXEC_COMPILE_ERROR,
    EXEC_RUNTIME_ERROR,
} xl_exec_result;

extern xl_vm vm;

void xl_vm_init(xl_vm_config config);
void xl_vm_shutdown();
xl_exec_result xl_vm_exec(const char* source);

#define MAX_OBJECT_COUNT 9999
#define MAX_STRINGS_COUNT 9999

#endif
