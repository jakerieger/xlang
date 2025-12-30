#include "xchunk.h"
#include "xalloc.h"
#include "xmem.h"
#include "xvalue.h"

void xl_chunk_init(xl_chunk* chunk) {
    chunk->count    = 0;
    chunk->capacity = 0;
    chunk->code     = NULL;
    chunk->lines    = NULL;
    xl_value_array_init(&chunk->constants);
}

void xl_chunk_write(xl_chunk* chunk, u8 byte, u64 line) {
    if (chunk->capacity < chunk->count + 1) {
        const i32 old_cap = chunk->capacity;
        chunk->capacity   = XL_GROW_CAPACITY(old_cap);
        chunk->code       = XL_GROW_ARRAY(u8, chunk->code, old_cap, chunk->capacity);
        chunk->lines      = XL_GROW_ARRAY(u64, chunk->lines, old_cap, chunk->capacity);
    }

    chunk->code[chunk->count]  = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;
}

void xl_chunk_free(xl_chunk* chunk) {
    XL_FREE_ARRAY(u8, chunk->code, chunk->capacity);
    XL_FREE_ARRAY(u64, chunk->lines, chunk->capacity);
    xl_value_array_free(&chunk->constants);
    xl_chunk_init(chunk);
}

i32 xl_chunk_add_constant(xl_chunk* chunk, xl_value value) {
    xl_value_array_write(&chunk->constants, value);
    return chunk->constants.count - 1;  // I don't remember why we're returning the index
}
