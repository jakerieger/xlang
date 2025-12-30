#include "xobject.h"
#include "xalloc.h"
#include "xtable.h"
#include "xutils.h"
#include "xvalue.h"
#include "xvm.h"

#define ALLOCATE_OBJ(type, obj_type) (type*)allocate_obj(sizeof(type), obj_type);

bool xl_obj_is_type(xl_value value, xl_obj_type type) {
    return VAL_IS_OBJ(value) && VAL_AS_OBJ(value)->type == type;
}

void xl_obj_print(xl_value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING: {
            xl_obj_string* val = (xl_obj_string*)value.as.obj;
            printf("%s", val->str);
        } break;
    }
}

static xl_obj* allocate_obj(size_t size, xl_obj_type type) {
    xl_obj* obj = (xl_obj*)xl_mem_realloc(NULL, 0, size);
    obj->type   = type;
    obj->next   = vm.objects;
    vm.objects  = obj;
    return obj;
}

static xl_obj_string* allocate_str(char* chars, i32 length, u32 hash) {
    xl_obj_string* str = ALLOCATE_OBJ(xl_obj_string, OBJ_STRING);
    str->length        = length;
    str->str           = chars;
    str->hash          = hash;
    xl_table_set(&vm.strings, str, NULL_VAL);

    return str;
}

xl_obj_string* xl_obj_str_take(char* chars, i32 length) {
    u32 hash                = xl_hash_string(chars, length);
    xl_obj_string* interned = xl_table_find_str(&vm.strings, chars, length, hash);
    if (interned != NULL) {
        XL_FREE_ARRAY(char, chars, length + 1);
        return interned;
    }

    return allocate_str(chars, length, hash);
}

xl_obj_string* xl_obj_str_copy(const char* chars, i32 length) {
    u32 hash                = xl_hash_string(chars, length);
    xl_obj_string* interned = xl_table_find_str(&vm.strings, chars, length, hash);
    if (interned != NULL)
        return interned;

    char* heap_chars = XL_ALLOCATE(char, length + 1);
    if (!heap_chars) {
        xl_error(XL_ERR_ALLOCATION_FAILED, "failed to allocate heap memory for string");
    }

    memcpy(heap_chars, chars, length);
    heap_chars[length] = '\0';

    return allocate_str(heap_chars, length, hash);
}
