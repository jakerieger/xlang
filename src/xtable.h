#ifndef X_TABLE_H
#define X_TABLE_H

#include "xalloc.h"
#include "xcommon.h"
#include "xvalue.h"

#define TABLE_MAX_LOAD 0.75

typedef struct xl_table_entry xl_table_entry;

struct xl_table_entry {
    xl_obj_string* key;
    xl_value value;
};

typedef struct {
    u64 count;
    u64 capacity;
    xl_table_entry* entries;
} xl_table;

void xl_table_init(xl_table* table);
void xl_table_free(xl_table* table);
bool xl_table_get(const xl_table* table, xl_obj_string* key, xl_value* value);
bool xl_table_set(xl_table* table, xl_obj_string* key, xl_value value);
bool xl_table_delete(xl_table* table, xl_obj_string* key);
void xl_table_add_all(xl_table* src, xl_table* dst);
xl_obj_string* xl_table_find_str(const xl_table* table, const char* chars, i32 length, u32 hash);

#endif
