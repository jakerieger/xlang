#include "xvm.h"
#include "xalloc.h"
#include "xchunk.h"
#include "xcommon.h"
#include "xmem.h"
#include "xcompiler.h"
#include "xobject.h"
#include "xstack.h"
#include "xtable.h"
#include "xutils.h"
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

    xl_table_init(&vm.strings);
    xl_table_init(&vm.globals);
}

void xl_vm_shutdown() {
    xl_table_free(&vm.strings);
    xl_table_free(&vm.globals);
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
    xl_obj_string* result = xl_obj_str_copy(str, length);
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
    xl_stack_reset(&vm.stack);
    xl_error(XL_ERR_EXEC_RUNTIME, "[line %d] in script\n", line);
}

#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(value_type, op)                                                                                      \
    do {                                                                                                               \
        if (!VAL_IS_NUMBER(peek(0)) || !VAL_IS_NUMBER(peek(1))) {                                                      \
            runtime_error("operands must be numbers.");                                                                \
            return EXEC_RUNTIME_ERROR;                                                                                 \
        }                                                                                                              \
        f64 b = VAL_AS_NUMBER(xl_stack_pop(&vm.stack));                                                                \
        f64 a = VAL_AS_NUMBER(xl_stack_pop(&vm.stack));                                                                \
        xl_stack_push(&vm.stack, value_type(a op b));                                                                  \
    } while (XL_FALSE)

//====================================================================================================================//

static xl_exec_result run() {
    for (;;) {
        u8 instruction;
        switch (instruction = READ_BYTE()) {
            case OP_RETURN: {
                xl_value_print(xl_stack_pop(&vm.stack));
                printf("\n");
                return EXEC_OK;
            }
            case OP_CONSTANT: {
                const xl_value constant = READ_CONSTANT();
                xl_stack_push(&vm.stack, constant);
                break;
            }
            case OP_NULL: {
                xl_stack_push(&vm.stack, NULL_VAL);
                break;
            }
            case OP_TRUE: {
                xl_stack_push(&vm.stack, BOOL_VAL(XL_TRUE));
                break;
            }
            case OP_FALSE: {
                xl_stack_push(&vm.stack, BOOL_VAL(XL_FALSE));
                break;
            }
            case OP_EQUAL: {
                xl_value b = xl_stack_pop(&vm.stack);
                xl_value a = xl_stack_pop(&vm.stack);
                xl_stack_push(&vm.stack, BOOL_VAL(xl_value_equal(a, b)));
                break;
            }
            case OP_GREATER: {
                BINARY_OP(BOOL_VAL, >);
                break;
            }
            case OP_LESS: {
                printf("Called run()\n");
                BINARY_OP(BOOL_VAL, <);
                break;
            }
            case OP_NEGATE: {
                if (!VAL_IS_NUMBER(peek(0))) {
                    runtime_error("operand must be a number");
                    return EXEC_RUNTIME_ERROR;
                }
                // modify in place
                (vm.stack.top - 1)->as.number = -((vm.stack.top - 1)->as.number);
                break;
            }
            case OP_NOT: {
                xl_stack_push(&vm.stack, BOOL_VAL(is_falsy(xl_stack_pop(&vm.stack))));
                break;
            }
            case OP_ADD: {
                if (OBJ_IS_STRING(peek(0)) && OBJ_IS_STRING(peek(1))) {
                    concatenate();
                } else if (VAL_IS_NUMBER(peek(0)) && VAL_IS_NUMBER(peek(1))) {
                    const f64 b = VAL_AS_NUMBER(xl_stack_pop(&vm.stack));
                    const f64 a = VAL_AS_NUMBER(xl_stack_pop(&vm.stack));
                    xl_stack_push(&vm.stack, NUMBER_VAL(a + b));
                } else {
                    runtime_error("operands must be of matching types");
                    return EXEC_RUNTIME_ERROR;
                }

                break;
            }
            case OP_SUBTRACT: {
                BINARY_OP(NUMBER_VAL, -);
                break;
            }
            case OP_MULTIPLY: {
                BINARY_OP(NUMBER_VAL, *);
                break;
            }
            case OP_DIVIDE: {
                BINARY_OP(NUMBER_VAL, /);
                break;
            }
            case OP_MOD: {
                if (!VAL_IS_NUMBER(peek(0)) || !VAL_IS_NUMBER(peek(1))) {
                    runtime_error("operands must be numbers");
                    return EXEC_RUNTIME_ERROR;
                }

                f64 b = VAL_AS_NUMBER(xl_stack_pop(&vm.stack));
                f64 a = VAL_AS_NUMBER(xl_stack_pop(&vm.stack));
                xl_stack_push(&vm.stack, NUMBER_VAL(fmod(a, b)));

                break;
            }
            default: {
                xl_error(XL_ERR_EXEC_RUNTIME, "unknown instruction (%n)", (i32)instruction);
            }
        }
    }
}

//====================================================================================================================//

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP

xl_exec_result xl_vm_exec(const char* source) {
    xl_chunk chunk;
    xl_chunk_init(&chunk);

    if (!xl_compile(source, &chunk)) {
        xl_alloc_clear(vm.mem.permanent);
        return EXEC_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip    = vm.chunk->code;

    const xl_exec_result result = run();

    xl_chunk_cleanup(&chunk);
    xl_alloc_clear(vm.mem.permanent);

    return result;
}
