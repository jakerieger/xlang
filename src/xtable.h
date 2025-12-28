#ifndef X_TABLE_H
#define X_TABLE_H

#include "xcommon.h"
#include "xvalue.h"

typedef struct {
    xl_obj_string* key;
    xl_value value;
} xl_table_entry;

typedef struct {
    u64 cap;
    u64 count;
    xl_table_entry* entries;
} xl_table;

void xl_table_init(xl_allocator* alloc, xl_table* table, size_t max_capacity);
void xl_table_clear(xl_table* table);
bool xl_table_get(const xl_table* table, xl_obj_string* key, xl_value* value);
bool xl_table_set(xl_table* table, xl_obj_string* key, xl_value value);
bool xl_table_delete(xl_table* table, xl_obj_string* key);
void xl_table_add_all(xl_table* src, xl_table* dst);
xl_obj_string* xl_table_find_string(const xl_table* table, const char* str, i32 length, u32 hash);

#endif
