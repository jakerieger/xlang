#include "xtable.h"
#include "xmem.h"
#include "xvalue.h"
#include "xobject.h"

void xl_table_init(xl_table* table) {
    table->count    = 0;
    table->capacity = 0;
    table->entries  = NULL;
}

void xl_table_free(xl_table* table) {
    XL_FREE_ARRAY(xl_table_entry, table->entries, table->capacity);
    xl_table_init(table);
}

static xl_table_entry* find_entry(xl_table_entry* entries, i32 capacity, xl_obj_string* key) {
    u32 index                 = key->hash % capacity;
    xl_table_entry* tombstone = NULL;

    for (;;) {
        xl_table_entry* entry = &entries[index];
        if (entry->key == NULL) {
            if (VAL_IS_NULL(entry->value)) {
                return tombstone != NULL ? tombstone : entry;
            } else {
                if (tombstone == NULL)
                    tombstone = entry;
            }
        } else if (entry->key == key) {
            return entry;
        }

        index = (index + 1) % capacity;
    }
}

static void adjust_capacity(xl_table* table, i32 capacity) {
    xl_table_entry* entries = XL_ALLOCATE(xl_table_entry, capacity);
    for (i32 i = 0; i < capacity; i++) {
        entries[i].key   = NULL;
        entries[i].value = NULL_VAL;
    }

    table->count = 0;
    for (i32 i = 0; i < table->capacity; i++) {
        const xl_table_entry* entry = &table->entries[i];
        if (entry->key != NULL) {
            xl_table_entry* dst = find_entry(entries, capacity, entry->key);
            dst->key            = entry->key;
            dst->value          = entry->value;
            // Each time we find a non-tombstone entry, we increment count
            table->count++;
        }
    }

    XL_FREE_ARRAY(xl_table_entry, table->entries, table->capacity);
    table->entries  = entries;
    table->capacity = capacity;
}

bool xl_table_get(const xl_table* table, xl_obj_string* key, xl_value* value) {
    if (table->count == 0)
        return XL_FALSE;

    const xl_table_entry* entry = find_entry(table->entries, table->capacity, key);
    if (entry->key == NULL)
        return XL_FALSE;

    *value = entry->value;
    return XL_TRUE;
}

bool xl_table_set(xl_table* table, xl_obj_string* key, xl_value value) {
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
        const i32 capacity = XL_GROW_CAPACITY(table->capacity);
        adjust_capacity(table, capacity);
    }

    xl_table_entry* entry = find_entry(table->entries, table->capacity, key);
    const bool is_new_key = entry->key == NULL;

    // Include tombstones in our count
    if (is_new_key && VAL_IS_NULL(entry->value))
        table->count++;

    entry->key   = key;
    entry->value = value;

    return is_new_key;
}

bool xl_table_delete(xl_table* table, xl_obj_string* key) {
    if (table->count == 0)
        return XL_FALSE;

    xl_table_entry* entry = find_entry(table->entries, table->capacity, key);
    if (entry->key == NULL)
        return XL_FALSE;

    // Place tombstone in entry
    entry->key   = NULL;
    entry->value = BOOL_VAL(XL_TRUE);

    return XL_TRUE;
}

void xl_table_add_all(xl_table* src, xl_table* dst) {
    for (i32 i = 0; i < src->capacity; i++) {
        const xl_table_entry* entry = &src->entries[i];
        if (entry->key != NULL) {
            xl_table_set(dst, entry->key, entry->value);
        }
    }
}

xl_obj_string* xl_table_find_str(const xl_table* table, const char* chars, i32 length, u32 hash) {
    if (table->count == 0)
        return NULL;

    u32 index = hash % table->capacity;
    for (;;) {
        const xl_table_entry* entry = &table->entries[index];
        if (entry->key == NULL) {
            // Stop if we find an empty non-tombstone entry
            if (VAL_IS_NULL(entry->value))
                return NULL;
        } else if (entry->key->length == length && entry->key->hash == hash &&
                   memcmp(entry->key->str, chars, length) == 0) {
            return entry->key;
        }

        index = (index + 1) % table->capacity;
    }
}
