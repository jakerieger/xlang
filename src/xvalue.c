#include "xvalue.h"
#include "xalloc.h"

bool xl_value_equal(xl_value a, xl_value b) {
    if (a.type != b.type)
        return XL_FALSE;

    switch (a.type) {
        case VAL_BOOL:
            return VAL_AS_BOOL(a) == VAL_AS_BOOL(b);
        case VAL_NUMBER:
            return VAL_AS_NUMBER(a) == VAL_AS_NUMBER(b);
        case VAL_OBJECT:
            return VAL_AS_OBJ(a) == VAL_AS_OBJ(b);
        case VAL_NULL:
            return XL_TRUE;
        default:
            return XL_FALSE;
    }
}

void xl_value_init_array(xl_allocator* alloc, xl_value_array* array, size_t max_capacity) {
    array->count  = 0;
    array->cap    = max_capacity;
    array->values = XL_ALLOC_ARRAY(alloc, xl_value, max_capacity);
}

i32 xl_value_write_array(xl_value_array* array, xl_value value) {
    if (array->count + 1 > array->cap) {
        return xl_error("failed to write to array, memory full");
    }
    array->values[++array->count] = value;
    return XL_OK;
}

void xl_value_print(xl_value value) {
    switch (value.type) {
        case VAL_BOOL:
            printf(VAL_AS_BOOL(value) ? "true" : "false");
            break;
        case VAL_NULL:
            printf("null");
            break;
        case VAL_NUMBER:
            printf("%g", VAL_AS_NUMBER(value));
            break;
        case VAL_OBJECT:
            // printObject(value);
            break;
        default:
            return;  // Unreachable
    }
}
