#include "xobject.h"
#include "xalloc.h"
#include "xtable.h"
#include "xvalue.h"
#include "xvm.h"

static u32 hash_string(const char* key, i32 length) {
    u32 hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

static xl_obj* make_obj(xl_allocator* alloc, size_t size, xl_obj_type type) {
    xl_obj* obj = (xl_obj*)xl_alloc_push(alloc, size, XL_FALSE);
    obj->type   = type;
    obj->next   = vm.objects;
    vm.objects  = obj;
    return obj;
}

static xl_obj_string* make_obj_str(xl_allocator* alloc, char* str, i32 length, u32 hash) {
    xl_obj_string* obj_str = (xl_obj_string*)make_obj(alloc, sizeof(xl_obj_string), OBJ_STRING);
    obj_str->length        = length;
    obj_str->str           = str;
    obj_str->hash          = hash;
    xl_table_set(&vm.strings, obj_str, NULL_VAL);

    return obj_str;
}

xl_obj_string* xl_obj_copy_string(xl_allocator* alloc, const char* str, i32 length) {
    u32 hash                = hash_string(str, length);
    xl_obj_string* interned = xl_table_find_string(&vm.strings, str, length, hash);
    if (interned)
        return interned;

    char* alloc_chars = XL_ALLOC_ARRAY(alloc, char, length + 1);
    if (!alloc_chars) {
        xl_error("failed to allocate string buffer");
        return NULL;
    }

    memcpy(alloc_chars, str, length);
    return make_obj_str(alloc, alloc_chars, length, hash);
}

void xl_obj_print(xl_value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", OBJ_AS_CSTRING(value));
            break;
    }
}
