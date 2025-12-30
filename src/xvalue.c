#include "xvalue.h"
#include "xmem.h"

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

void xl_value_array_init(xl_value_array* array) {
    array->count  = 0;
    array->cap    = 0;
    array->values = NULL;
}

void xl_value_array_write(xl_value_array* array, xl_value value) {
    if (array->cap < array->count + 1) {
        const u64 old_cap = array->cap;
        array->cap        = XL_GROW_CAPACITY(old_cap);
        array->values     = XL_GROW_ARRAY(xl_value, array->values, old_cap, array->cap);
    }

    array->values[array->count] = value;
    array->count++;
}

void xl_value_array_free(xl_value_array* array) {
    XL_FREE_ARRAY(xl_value, array->values, array->cap);
    xl_value_array_init(array);
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
