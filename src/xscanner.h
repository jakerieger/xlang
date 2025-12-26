#ifndef X_SCANNER_H
#define X_SCANNER_H

#include "xcommon.h"
#include "xalloc.h"

typedef enum {
    TOKEN_PLUS,                  // | +
    TOKEN_MINUS,                 // | -
    TOKEN_ASTERISK,              // | *
    TOKEN_DOUBLE_ASTERISK,       // | **
    TOKEN_FORWARD_SLASH,         // | /
    TOKEN_DOUBLE_FORWARD_SLASH,  // | //
    TOKEN_COMMA,                 // | ,
    TOKEN_MODULO,                // | %
    TOKEN_ASSIGNMENT,            // | =
    TOKEN_IDENTIFIER,            // | my_var, x
    TOKEN_LEFT_PAREN,            // | (
    TOKEN_RIGHT_PAREN,           // | )
    TOKEN_NUMBER,                // | 0.4, 1000
    TOKEN_KEYWORD,               // | print
    TOKEN_EOF,                   // | 0x-1
} xl_token_type;

typedef struct {
    xl_token_type type;

    union {
        f64 n;
        char* s;
    } value;
} xl_token;

void xl_token_print(xl_token* token);

typedef struct {
    xl_allocator* alloc;
    char* source;
    u64 source_len;
    u64 pos;
} xl_scanner;

xl_scanner* xl_scanner_create(xl_allocator* alloc, const char* input);
bool xl_scanner_emit(xl_scanner* scanner, xl_token* token_out);
const char* xl_token_type_to_str(xl_token_type type);

#define INVALID_CHAR -0x2

#endif
