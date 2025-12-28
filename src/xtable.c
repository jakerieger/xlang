#include "xtable.h"
#include "xobject.h"

void xl_table_init(xl_allocator* alloc, xl_table* table, size_t max_capacity) {
    table->count   = 0;
    table->cap     = max_capacity;
    table->entries = XL_ALLOC_ARRAY(alloc, xl_table_entry, max_capacity);
}

static xl_table_entry* find_entry(xl_table_entry* entries, i32 capacity, xl_obj_string* key) {
    u32 index = key->hash % capacity;

    return NULL;
}

bool xl_table_get(const xl_table* table, xl_obj_string* key, xl_value* value) {
    return XL_TRUE;
}

bool xl_table_set(xl_table* table, xl_obj_string* key, xl_value value) {
    return XL_TRUE;
}

bool xl_table_delete(xl_table* table, xl_obj_string* key) {
    return XL_TRUE;
}

void xl_table_add_all(xl_table* src, xl_table* dst) {}

xl_obj_string* xl_tble_find_string(const xl_table* table, const char* str, i32 length, u32 hash) {
    return NULL;
}

void xl_table_clear(xl_table* table) {
    table->count = 0;
}
