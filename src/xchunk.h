#ifndef X_CHUNK_H
#define X_CHUNK_H

#include "xcommon.h"
#include "xvalue.h"

#define MAX_CONSTANTS_SIZE 1000
#define MAX_CODE_SIZE XL_MB(4)
#define MAX_LINES_SIZE 9999

typedef enum {
    OP_CONSTANT,
    OP_NULL,
    OP_TRUE,
    OP_FALSE,
    OP_NOT,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_MOD,
    OP_RETURN,
} xl_opcode;

typedef struct {
    u64 count;
    u8* code;
    u64* lines;
    xl_value_array constants;
} xl_chunk;

void xl_chunk_init(xl_allocator* alloc, xl_chunk* chunk);
i32 xl_chunk_write(xl_chunk* chunk, u8 byte, u64 line);
i32 xl_chunk_add_constant(xl_chunk* chunk, xl_value value);

#endif
