#include "xmem.h"
#include "xerr.h"
#include "xvalue.h"
#include "xobject.h"
#include "xvm.h"

void xl_vm_mem_init(xl_vm_mem* mem, size_t size_perm, size_t size_gen, size_t size_temp) {
    mem->permanent  = xl_alloc_create(size_perm);
    mem->generation = xl_alloc_create(size_gen);
    mem->temporary  = xl_alloc_create(size_temp);
}

void xl_vm_mem_destroy(xl_vm_mem* mem) {
    xl_alloc_destroy(mem->permanent);
    xl_alloc_destroy(mem->generation);
    xl_alloc_destroy(mem->temporary);
}

void* xl_mem_realloc(void* ptr, size_t old_size, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }

    void* result = realloc(ptr, new_size);
    if (result == NULL)
        xl_error(XL_ERR_ALLOCATION_FAILED, "failed to reallocate memory");

    return result;
}

void xl_mem_free_objects() {
    xl_obj* obj = vm.objects;
    while (obj != NULL) {
        xl_obj* next = obj->next;
        xl_mem_free_object(obj);
        obj = next;
    }
}

static void xl_mem_free_object(xl_obj* obj) {
    switch (obj->type) {
        case OBJ_STRING: {
            const xl_obj_string* str = (xl_obj_string*)obj;
            XL_FREE_ARRAY(char, str->str, str->length + 1);
            XL_FREE(xl_obj_string, obj);
            break;
        }
    }
}
