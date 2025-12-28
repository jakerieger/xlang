#include "xvm.h"
#include "xalloc.h"
#include "xchunk.h"
#include "xmem.h"
#include "xcompiler.h"
#include "xobject.h"
#include "xstack.h"
#include "xtable.h"
#include "xvalue.h"

/*
 * GLOBAL VM instance
 */
xl_vm vm;

void xl_vm_init(xl_vm_config config) {
    xl_vm_mem_init(&vm.mem, config.mem_size_permanent, config.mem_size_generation, config.mem_size_temporary);

    xl_stack_init(vm.mem.permanent, &vm.stack, config.stack_size);

    vm.objects = XL_ALLOC_ARRAY(vm.mem.permanent, xl_obj, MAX_OBJECT_COUNT);
    assert(vm.objects);

    xl_table_init(vm.mem.permanent, &vm.strings, MAX_STRINGS_COUNT);
}

void xl_vm_shutdown() {
    xl_vm_mem_destroy(&vm.mem);
}

static xl_value peek(i32 distance) {
    return vm.stack.top[-1 - distance];
}

static bool is_falsy(xl_value value) {
    return VAL_IS_NULL(value) || (VAL_IS_BOOL(value) && !VAL_AS_BOOL(value));
}

static void concatenate() {
    const xl_obj_string* b = OBJ_AS_STRING(xl_stack_pop(&vm.stack));
    const xl_obj_string* a = OBJ_AS_STRING(xl_stack_pop(&vm.stack));
    const i32 length       = a->length + b->length;
    char* str              = XL_ALLOC_ARRAY(vm.mem.permanent, char, length + 1);
    memcpy(str, a->str, a->length);
    memcpy(str + a->length, b->str, b->length);
    str[length]           = '\0';
    xl_obj_string* result = xl_obj_copy_string(vm.mem.permanent, str, length);
    xl_stack_push(&vm.stack, OBJ_VAL(result));
}

static void runtime_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputs("\n", stderr);
    const size_t instruction = vm.ip - vm.chunk->code - 1;
    const int line           = vm.chunk->lines[instruction];
    xl_error("[line %d] in script\n", line);
    xl_stack_reset(&vm.stack);
}

#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(value_type, op)                                                                                      \
    do {                                                                                                               \
        if (!VAL_IS_NUMBER(peek(0)) || !VAL_IS_NUMBER(peek(1))) {                                                      \
            runtime_error("Operands must be numbers.");                                                                \
            return EXEC_RUNTIME_ERROR;                                                                                 \
        }                                                                                                              \
        f64 b = VAL_AS_NUMBER(xl_stack_pop(&vm.stack));                                                                \
        f64 a = VAL_AS_NUMBER(xl_stack_pop(&vm.stack));                                                                \
        xl_stack_push(&vm.stack, value_type(a op b));                                                                  \
    } while (false)

//====================================================================================================================//

static xl_exec_result run() {
    u8 instruction;
    switch (instruction = READ_BYTE()) {
        case OP_RETURN:
        case OP_CONSTANT:
        case OP_NULL:
        case OP_TRUE:
        case OP_FALSE:
        case OP_EQUAL:
        case OP_GREATER:
        case OP_LESS:
        case OP_NEGATE:
        case OP_NOT:
        case OP_ADD:
        case OP_SUBTRACT:
        case OP_MULTIPLY:
        case OP_DIVIDE:
        case OP_MOD:
        default: {
            xl_error("unknown instruction (%n)", (i32)instruction);
            return EXEC_RUNTIME_ERROR;
        }
    }

    return EXEC_OK;
}

//====================================================================================================================//

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP

xl_exec_result xl_vm_exec(const char* source) {
    xl_chunk chunk;
    xl_chunk_init(vm.mem.permanent, &chunk);

    if (!xl_compile(source, &chunk)) {
        xl_alloc_clear(vm.mem.permanent);
        return EXEC_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip    = vm.chunk->code;

    const xl_exec_result result = run();
    xl_alloc_clear(vm.mem.permanent);

    return result;
}
