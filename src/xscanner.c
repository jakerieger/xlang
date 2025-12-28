#include "xscanner.h"
#include "xalloc.h"
#include "xcommon.h"
#include <stdio.h>
#include <string.h>

static bool is_whitespace(char c) {
    return c <= ' ';
}

static bool is_numeric(char c) {
    return (c >= '0') && (c <= '9');
}

static bool is_alnum(char c) {
    bool is_uppercase = (c >= 'A') && (c <= 'Z');
    bool is_lowercase = (c >= 'a') && (c <= 'z');
    return is_uppercase || is_lowercase || is_numeric(c);
}

static char advance(xl_scanner* scanner) {
    if (scanner->pos + 1 > scanner->source_len) {
        xl_error("tried to advance past source length (%lu > %lu)\n", scanner->pos + 1, scanner->source_len);
        return INVALID_CHAR;
    }

    return scanner->source[++scanner->pos];
}

static char peek(xl_scanner* scanner) {
    if (scanner->pos + 1 > scanner->source_len) {
        xl_error("tried to advance past source length (%lu > %lu)\n", scanner->pos + 1, scanner->source_len);
        return INVALID_CHAR;
    }

    return scanner->source[scanner->pos + 1];
}

static char current(xl_scanner* scanner) {
    return scanner->source[scanner->pos];
}

static void advance_pos(xl_scanner* scanner) {
    ++scanner->pos;
}

xl_scanner* xl_scanner_create(xl_allocator* alloc, const char* input) {
    xl_scanner* out = (xl_scanner*)xl_alloc_push(alloc, sizeof(xl_scanner), XL_FALSE);
    if (!out) {
        xl_error("failed to allocate scanner");
        return NULL;
    }

    u32 input_len = strlen(input) + 1;
    out->source   = (char*)xl_alloc_push(alloc, input_len, XL_FALSE);
    strncpy(out->source, input, input_len);

    if (!out->source) {
        xl_error("failed to copy source to scanner");
        return NULL;
    }

    out->pos        = 0;
    out->source_len = input_len - 1;
    out->alloc      = alloc;

    return out;
}

static xl_token make_number_token(f64 val) {
    xl_token token;
    token.type    = TOKEN_NUMBER;
    token.value.n = val;
    return token;
}

static bool make_string_token(xl_allocator* alloc, const char* val, xl_token_type type, xl_token* token_out) {
    token_out->type = type;

    u32 val_len        = strlen(val);
    u32 buf_len        = val_len + 1;  // + null term
    token_out->value.s = (char*)xl_alloc_push(alloc, buf_len, XL_FALSE);
    if (!token_out->value.s) {
        xl_error("failed to allocate token string memory");
        return XL_FALSE;
    }
    strncpy(token_out->value.s, val, buf_len);

    return XL_TRUE;
}

static xl_token make_nonvalue_token(xl_token_type type) {
    xl_token token;
    token.type = type;
    return token;
}

#define MAKE_ERR_TOKEN make_nonvalue_token(TOKEN_EOF)

#define MAX_DIGIT_COUNT 308
#define MAX_IDENT_LENGTH 64
#define MAX_STRING_LENGTH 1024

static xl_token parse_number(xl_scanner* scanner) {
    /*
     * All numbers in xlang are represented internally as doubles.
     * They can contain a maximum digit count of 308.
     */
    char buffer[MAX_DIGIT_COUNT] = {'\0'};
    u64 buffer_pos               = 0;

    while (scanner->pos < scanner->source_len) {
        char c = current(scanner);

        if (!is_numeric(c) && c != '.') {
            break;
        }

        if ((size_t)buffer_pos >= MAX_DIGIT_COUNT) {
            xl_error("number contains too many digits (max 64)");
            return MAKE_ERR_TOKEN;
        }

        buffer[buffer_pos++] = c;
        advance_pos(scanner);
    }

    f64 val = strtod(buffer, NULL);

    return make_number_token(val);
}

static bool is_keyword(const char* ident) {
    XL_UNUSED(ident);
    return XL_FALSE;
}

static xl_token parse_identifier(xl_scanner* scanner) {
    char buffer[MAX_IDENT_LENGTH] = {'\0'};
    u64 buffer_pos                = 0;

    while (scanner->pos < scanner->source_len) {
        char c = current(scanner);

        if (!is_alnum(c) && c != '_') {
            break;
        }

        if ((size_t)buffer_pos >= MAX_DIGIT_COUNT) {
            xl_error("number contains too many digits (max 64)");
            return MAKE_ERR_TOKEN;
        }

        buffer[buffer_pos++] = c;
        advance_pos(scanner);
    }

    xl_token out;
    xl_token_type type = is_keyword(buffer) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    bool result        = make_string_token(scanner->alloc, buffer, type, &out);
    if (!result) {
        xl_error("failed to create string token");
        return MAKE_ERR_TOKEN;
    }

    return out;
}

bool xl_scanner_emit(xl_scanner* scanner, xl_token* token_out) {
    if (scanner->pos >= scanner->source_len - 1) {
        *token_out = make_nonvalue_token(TOKEN_EOF);
        return XL_TRUE;
    }

    char c = current(scanner);

    while (is_whitespace(c)) {
        c = advance(scanner);
    }

    if (c == '#') {
        while (c != '\n' && c != EOF) {
            c = advance(scanner);
        }
    }
    if (c == '\n') {
        c = advance(scanner);
    }

    xl_token_type type = TOKEN_EOF;
    switch (c) {
        case '(':
            type = TOKEN_LEFT_PAREN;
            break;
        case ')':
            type = TOKEN_RIGHT_PAREN;
            break;
        case ',':
            type = TOKEN_COMMA;
            break;
        case '=':
            type = TOKEN_ASSIGNMENT;
            break;
        case '+':
            type = TOKEN_PLUS;
            break;
        case '-':
            type = TOKEN_MINUS;
            break;
        case '%':
            type = TOKEN_MODULO;
            break;
        case '*': {
            if (peek(scanner) == '*') {
                type = TOKEN_DOUBLE_ASTERISK;
                c    = advance(scanner);
            } else {
                type = TOKEN_ASTERISK;
            }
        } break;
        case '/': {
            if (peek(scanner) == '/') {
                type = TOKEN_DOUBLE_FORWARD_SLASH;
                c    = advance(scanner);
            } else {
                type = TOKEN_FORWARD_SLASH;
            }
        } break;
        case EOF:
            type = TOKEN_EOF;
            break;
        default: {
            if (is_numeric(c)) {
                *token_out = parse_number(scanner);
                return XL_TRUE;
            }

            if (is_alnum(c)) {
                *token_out = parse_identifier(scanner);
                return XL_TRUE;
            }

            xl_error("invalid token: '%c' (%d)\n", c, (i32)c);
            return XL_FALSE;
        }
    }

    *token_out = make_nonvalue_token(type);
    advance_pos(scanner);

    return XL_TRUE;
}

void xl_token_print(xl_token* token) {
    const char* type_str = xl_token_type_to_str(token->type);
    printf("Token: '%s' ", type_str);
    if (token->type == TOKEN_NUMBER) {
        printf("(%f)", token->value.n);
    } else if (token->type == TOKEN_IDENTIFIER || token->type == TOKEN_KEYWORD) {
        printf("(%s)", token->value.s);
    }
    printf("\n");
}

const char* xl_token_type_to_str(xl_token_type type) {
    switch (type) {
        case TOKEN_PLUS:
            return "+";
        case TOKEN_MINUS:
            return "-";
        case TOKEN_ASTERISK:
            return "*";
        case TOKEN_DOUBLE_ASTERISK:
            return "**";
        case TOKEN_FORWARD_SLASH:
            return "/";
        case TOKEN_DOUBLE_FORWARD_SLASH:
            return "//";
        case TOKEN_COMMA:
            return ",";
        case TOKEN_ASSIGNMENT:
            return "=";
        case TOKEN_IDENTIFIER:
            return "identifier";
        case TOKEN_LEFT_PAREN:
            return "(";
        case TOKEN_RIGHT_PAREN:
            return ")";
        case TOKEN_NUMBER:
            return "number";
        case TOKEN_KEYWORD:
            return "keyword";
        case TOKEN_EOF:
            return "EOF";
        case TOKEN_MODULO:
            return "%";
    }
    return "";
}
