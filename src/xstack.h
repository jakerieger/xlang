#ifndef X_STACK_H
#define X_STACK_H

#include "xalloc.h"
#include "xvalue.h"

typedef struct {
    u64 cap;
    u64 count;
    xl_value* stack;
    xl_value* top;
} xl_stack;

void xl_stack_init(xl_allocator* alloc, xl_stack* stack, size_t size);
i32 xl_stack_push(xl_stack* stack, xl_value value);
xl_value xl_stack_pop(xl_stack* stack);
void xl_stack_reset(xl_stack* stack);

#endif
