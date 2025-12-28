#ifndef X_VALUE_H
#define X_VALUE_H

/**
 * Defines common value types used by xlang
 */

#include "xcommon.h"
#include "xalloc.h"

typedef struct xl_obj xl_obj;
typedef struct xl_obj_string xl_obj_string;

typedef enum {
    VAL_BOOL,
    VAL_NULL,
    VAL_NUMBER,
    VAL_OBJECT,
} xl_value_type;

typedef struct {
    xl_value_type type;

    union {
        bool boolean;
        f64 number;
        xl_obj* obj;
    } as;
} xl_value;

typedef struct {
    u64 cap;
    u64 count;
    xl_value* values;
} xl_value_array;

#define VAL_IS_BOOL(v) ((v).type == VAL_BOOL)
#define VAL_IS_NULL(v) ((v).type == VAL_NULL)
#define VAL_IS_NUMBER(v) ((v).type == VAL_NUMBER)
#define VAL_IS_OBJ(v) ((v).type == VAL_OBJECT)

#define VAL_AS_OBJ(v) ((v).as.obj)
#define VAL_AS_BOOL(v) ((v).as.boolean)
#define VAL_AS_NUMBER(v) ((v).as.number)

#define NULL_VAL ((xl_value) {VAL_NULL, {.number = 0}})
#define BOOL_VAL(v) ((xl_value) {VAL_BOOL, {.boolean = (v)}})
#define NUMBER_VAL(v) ((xl_value) {VAL_NUMBER, {.number = (v)}})
#define OBJ_VAL(o) ((xl_value) {VAL_OBJECT, {.obj = (xl_obj*)(o)}})

bool xl_value_equal(xl_value a, xl_value b);
void xl_value_init_array(xl_allocator* alloc, xl_value_array* array, size_t max_capacity);
i32 xl_value_write_array(xl_value_array* array, xl_value value);
void xl_value_print(xl_value value);

#endif
