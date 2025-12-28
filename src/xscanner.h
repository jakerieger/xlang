#ifndef X_SCANNER_H
#define X_SCANNER_H

#include "xcommon.h"
#include "xalloc.h"

typedef enum {
    // Single-character tokens
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_LEFT_BRACE,
    TOKEN_RIGHT_BRACE,
    TOKEN_COMMA,
    TOKEN_DOT,
    TOKEN_MINUS,
    TOKEN_PLUS,
    TOKEN_SEMICOLON,
    TOKEN_FORWARD_SLASH,
    TOKEN_DOUBLE_FORWARD_SLASH,
    TOKEN_ASTERISK,
    TOKEN_DOUBLE_ASTERISK,
    TOKEN_PERCENT,
    // One or two character tokens
    TOKEN_BANG,
    TOKEN_BANG_EQUAL,
    TOKEN_EQUAL,
    TOKEN_EQUAL_EQUAL,
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    // Literals
    TOKEN_IDENTIFIER,
    TOKEN_STRING,
    TOKEN_NUMBER,
    // Keywords
    TOKEN_AND,
    TOKEN_CLASS,
    TOKEN_ELSE,
    TOKEN_FALSE,
    TOKEN_FOR,
    TOKEN_FN,
    TOKEN_IF,
    TOKEN_NULL,
    TOKEN_OR,
    TOKEN_PRINT,
    TOKEN_RETURN,
    TOKEN_THIS,
    TOKEN_TRUE,
    TOKEN_VAR,
    TOKEN_WHILE,
    // Signal tokens
    TOKEN_ERROR,
    TOKEN_EOF,
} xl_token_type;

typedef struct {
    xl_token_type type;
    const char* start;
    i32 length;
    i32 line;
} xl_token;

void xl_token_print(xl_token* token);
const char* xl_token_type_to_str(xl_token_type type);

void xl_scanner_init(const char* source);
xl_token xl_scanner_emit();

#endif
