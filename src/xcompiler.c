#include "xcompiler.h"
#include "xchunk.h"
#include "xerr.h"
#include "xobject.h"
#include "xscanner.h"
#include "xvalue.h"
#include <math.h>

typedef struct {
    xl_token current;
    xl_token previous;
    bool had_error;
    bool panic_mode;
} xl_parser;

typedef enum {
    PREC_NONE,
    PREC_ASSIGNMENT,  // =
    PREC_OR,          // or
    PREC_AND,         // and
    PREC_EQUALITY,    // == !=
    PREC_COMPARISON,  // < > <= >=
    PREC_TERM,        // + -
    PREC_FACTOR,      // * /
    PREC_UNARY,       // ! -
    PREC_CALL,        // . ()
    PREC_PRIMARY
} xl_op_prec;

typedef void (*xl_parse_fn)();

typedef struct {
    xl_parse_fn prefix;
    xl_parse_fn infix;
    xl_op_prec precedence;
} xl_parse_rule;

xl_parser parser;
xl_chunk* compiling_chunk;

static void expression();
static xl_parse_rule* get_rule(xl_token_type);
static void parse_precedence(xl_op_prec);

static xl_chunk* current_chunk() {
    return compiling_chunk;
}

static void error_at(const xl_token* token, const char* msg) {
    if (parser.panic_mode)
        return;

    parser.panic_mode = XL_TRUE;
    fprintf(stderr, "[line %d] error: ", token->line);

    if (token->type == TOKEN_EOF) {
        fprintf(stderr, " at end");
    } else if (token->type == TOKEN_ERROR) {
        //
    } else {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", msg);
    parser.had_error = XL_TRUE;
}

static void error(const char* msg) {
    error_at(&parser.previous, msg);
}

static void error_at_current(const char* msg) {
    error_at(&parser.current, msg);
}

static void advance() {
    parser.previous = parser.current;

    for (;;) {
        parser.current = xl_scanner_emit();
        if (parser.current.type != TOKEN_ERROR) {
            break;
        }

        error_at_current(parser.current.start);
    }
}

static void consume(const xl_token_type type, const char* msg) {
    if (parser.current.type == type) {
        advance();
        return;
    }

    error_at_current(msg);
}

static void emit_byte(const u8 byte) {
    xl_chunk_write(current_chunk(), byte, parser.previous.line);
}

static void emit_bytes(const u8 op, const u8 operand) {
    emit_byte(op);
    emit_byte(operand);
}

static void emit_return() {
    emit_byte(OP_RETURN);
}

static u8 make_constant(xl_value value) {
    const i32 constant = xl_chunk_add_constant(current_chunk(), value);
    if (constant > UINT8_MAX) {
        xl_panic(XL_ERR_OVER_CAPACITY, "too many constants in one chunk (maximum is 256");
    }

    return (u8)constant;
}

static void emit_constant(xl_value value) {
    emit_bytes(OP_CONSTANT, make_constant(value));
}

static void end_compiler() {
    emit_return();

    // TODO: Debug dissaembly
}

static void grouping() {
    expression();
    consume(TOKEN_RIGHT_PAREN, "expected closing ')'");
}

static void number() {
    f64 value = strtod(parser.previous.start, NULL);
    emit_constant(NUMBER_VAL(value));
}

static void string() {
    emit_constant(OBJ_VAL(xl_obj_str_copy(parser.previous.start + 1, parser.previous.length - 2)));
}

static void unary() {
    const xl_token_type op_type = parser.previous.type;
    parse_precedence(PREC_UNARY);

    switch (op_type) {
        case TOKEN_MINUS:
            emit_byte(OP_NEGATE);
            break;
        case TOKEN_BANG:
            emit_byte(OP_NOT);
            break;
        default:
            return;
    }
}

static void binary() {
    const xl_token_type op_type = parser.previous.type;
    xl_parse_rule* rule         = get_rule(op_type);
    parse_precedence((xl_op_prec)(rule->precedence + 1));

    switch (op_type) {
        case TOKEN_BANG_EQUAL:
            emit_bytes(OP_EQUAL, OP_NOT);
            break;
        case TOKEN_EQUAL_EQUAL:
            emit_byte(OP_EQUAL);
            break;
        case TOKEN_GREATER:
            emit_byte(OP_GREATER);
            break;
        case TOKEN_GREATER_EQUAL:
            emit_bytes(OP_LESS, OP_NOT);
            break;
        case TOKEN_LESS:
            emit_byte(OP_LESS);
            break;
        case TOKEN_LESS_EQUAL:
            emit_bytes(OP_GREATER, OP_NOT);
            break;
        case TOKEN_PLUS:
            emit_byte(OP_ADD);
            break;
        case TOKEN_MINUS:
            emit_byte(OP_SUBTRACT);
            break;
        case TOKEN_ASTERISK:
            emit_byte(OP_MULTIPLY);
            break;
        case TOKEN_PERCENT:
            emit_byte(OP_MOD);
            break;
        case TOKEN_FORWARD_SLASH:
            emit_byte(OP_DIVIDE);
            break;
        default:
            return;
    }
}

static void literal() {
    switch (parser.previous.type) {
        case TOKEN_FALSE:
            emit_byte(OP_FALSE);
            break;
        case TOKEN_NULL:
            emit_byte(OP_NULL);
            break;
        case TOKEN_TRUE:
            emit_byte(OP_TRUE);
            break;
        default:
            return;
    }
}

// clang-format off
xl_parse_rule rules[] = {
    [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
    [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
    [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
    [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
    [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FORWARD_SLASH] = {NULL,     binary, PREC_FACTOR},
    [TOKEN_ASTERISK]      = {NULL,     binary, PREC_FACTOR},
    [TOKEN_PERCENT]       = {NULL,     binary, PREC_FACTOR},
    [TOKEN_BANG]          = {unary,    NULL,   PREC_NONE},
    [TOKEN_BANG_EQUAL]    = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EQUAL_EQUAL]   = {NULL,     binary, PREC_EQUALITY},
    [TOKEN_GREATER]       = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_LESS]          = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL]    = {NULL,     binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
    [TOKEN_STRING]        = {string,   NULL,   PREC_NONE},
    [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
    [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FALSE]         = {literal,  NULL,   PREC_NONE},
    [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_FN]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_NULL]          = {literal,  NULL,   PREC_NONE},
    [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
    [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
    [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
    [TOKEN_TRUE]          = {literal,  NULL,   PREC_NONE},
    [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
    [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
    [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};
// clang-format on

static void expression() {
    parse_precedence(PREC_ASSIGNMENT);
}

static xl_parse_rule* get_rule(xl_token_type type) {
    return &rules[type];
}

static void parse_precedence(xl_op_prec prec) {
    advance();

    xl_parse_fn prefix_rule = get_rule(parser.previous.type)->prefix;
    if (!prefix_rule) {
        xl_panic(XL_ERR_EXPECTED_EXPRESSION,
                 "[line %d] at '%.*s' - expected expression",
                 parser.previous.line,
                 parser.previous.length,
                 parser.previous.start);
    }

    prefix_rule();

    while (prec <= get_rule(parser.current.type)->precedence) {
        advance();
        xl_parse_fn infix_rule = get_rule(parser.previous.type)->infix;
        infix_rule();
    }
}

bool xl_compile(const char* source, xl_chunk* chunk) {
    xl_scanner_init(source);
    compiling_chunk = chunk;

    parser.had_error  = XL_FALSE;
    parser.panic_mode = XL_FALSE;

    advance();
    expression();
    consume(TOKEN_EOF, "expected end of expression");
    end_compiler();

    return !parser.had_error;
}
