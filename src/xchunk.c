#include "xchunk.h"
#include "xalloc.h"
#include "xvalue.h"

void xl_chunk_init(xl_allocator* alloc, xl_chunk* chunk) {
    chunk->count = 0;
    chunk->code  = XL_ALLOC_ARRAY(alloc, u8, MAX_CODE_SIZE);
    chunk->lines = XL_ALLOC_ARRAY(alloc, u64, MAX_LINES_SIZE);
    xl_value_init_array(alloc, &chunk->constants, MAX_CONSTANTS_SIZE);
}

i32 xl_chunk_write(xl_chunk* chunk, u8 byte, u64 line) {
    if (chunk->count + 1 > MAX_CODE_SIZE) {
        return xl_error("failed to write chunk, maximum capacity reached");
    }

    chunk->code[chunk->count]  = byte;
    chunk->lines[chunk->count] = line;
    chunk->count++;

    return XL_OK;
}

i32 xl_chunk_add_constant(xl_chunk* chunk, xl_value value) {
    xl_value_write_array(&chunk->constants, value);
    return chunk->constants.count - 1;  // I don't remember why we're returning the index
}
