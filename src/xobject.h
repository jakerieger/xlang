#ifndef X_OBJECT_H
#define X_OBJECT_H

#include "xvalue.h"

typedef enum {
    OBJ_STRING,
} xl_obj_type;

struct xl_obj {
    xl_obj_type type;
    xl_obj* next;
};

struct xl_obj_string {
    xl_obj obj;
    i32 length;
    char* str;
    u32 hash;
};

#define OBJ_TYPE(v) (VAL_AS_OBJ(v)->type)
#define OBJ_IS_STRING(v) xl_obj_is_type(v, OBJ_STRING)
#define OBJ_AS_STRING(v) ((xl_obj_string*)VAL_AS_OBJ(v))
#define OBJ_AS_CSTRING(v) (((xl_obj_string*)VAL_AS_OBJ(v))->str)

static bool xl_obj_is_type(xl_value value, xl_obj_type type);
void xl_obj_print(xl_value value);
xl_obj_string* xl_obj_str_take(char* chars, i32 length);
xl_obj_string* xl_obj_str_copy(const char* chars, i32 length);

#endif
