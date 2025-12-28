#include "xstack.h"

void xl_stack_init(xl_allocator* alloc, xl_stack* stack, size_t size) {
    stack->count = 0;
    stack->cap   = size;
    stack->stack = XL_ALLOC_ARRAY(alloc, xl_value, size);
}

i32 xl_stack_push(xl_stack* stack, xl_value value) {
    if (stack->count + 1 > stack->cap) {
        xl_error("failed to push to stack, maximum capacity");
        return XL_FAIL;
    }

    *stack->top = value;
    stack->top++;

    return XL_OK;
}

xl_value xl_stack_pop(xl_stack* stack) {
    stack->top--;
    return *stack->top;
}

void xl_stack_reset(xl_stack* stack) {
    stack->top = stack->stack;
}
