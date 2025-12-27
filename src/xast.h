#ifndef X_AST_H
#define X_AST_H

#include "xalloc.h"
#include "xcommon.h"
#include "xscanner.h"

typedef enum {
    NODE_NUMBER,
    NODE_STRING,
    NODE_IDENTIFIER,
    NODE_BINARY_OP,
    NODE_FUNC_DEF,
    NODE_FUNC_CALL,
} xl_node_type;

typedef enum {
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_EXPONENTIATION,
    OP_FLOORDIV,
    OP_MODULUS,
    OP_ASSIGN,
} xl_binary_operand;

inline static const char* binop_to_str(xl_binary_operand);

typedef struct xl_ast_node xl_ast_node;

typedef struct {
    f64 value;
} xl_node_number;

typedef struct {
    char* value;
    u64 len;
} xl_node_string;

typedef struct {
    char* value;
    u64 len;
} xl_node_identifier;

/*
 * Binary operations include assignments (=)
 */
typedef struct {
    xl_ast_node* left;
    xl_ast_node* right;
    xl_binary_operand op;
} xl_node_binary_op;

typedef struct {
    char* name;
} xl_node_func_def;

typedef struct {
    char* name;
} xl_node_func_call;

struct xl_ast_node {
    xl_node_type type;

    union {
        xl_node_number num;
        xl_node_string str;
        xl_node_binary_op binop;
        xl_node_identifier ident;
        xl_node_func_call fcall;
        xl_node_func_def fdef;
    } value;
};

inline static void xl_node_print(xl_ast_node* node) {
    switch (node->type) {
        case NODE_NUMBER:
            printf("number (%f)\n", node->value.num.value);
            break;
        case NODE_BINARY_OP:
            printf("binary op (%s)\n", binop_to_str(node->value.binop.op));
            break;
        case NODE_IDENTIFIER:
            printf("identifier (%s)\n", node->value.ident.value);
            break;
        case NODE_STRING:
            printf("string (\"%s\")\n", node->value.str.value);
            break;
        case NODE_FUNC_DEF:
            printf("func call (%s)\n", node->value.fcall.name);
            break;
        case NODE_FUNC_CALL:
            printf("func call (%s)\n", node->value.fdef.name);
            break;
    }
}

inline static xl_ast_node* xl_node_make_val(xl_allocator* alloc, xl_node_type type, XL_ANY value) {
    assert(alloc);
    assert(value);

    u64 curr_mem_pos = alloc->pos;
    xl_ast_node* out = (xl_ast_node*)xl_alloc_push(alloc, sizeof(xl_ast_node), XL_FALSE);
    if (!out) {
        xl_perror("failed to allocate ast node");
        return NULL;
    }

    out->type = type;

    if (value) {
        if (type == NODE_NUMBER) {
            f64* v               = (f64*)value;
            out->value.num.value = *v;
        }

        if (type == NODE_IDENTIFIER || type == NODE_STRING) {
            u32 value_len          = strlen((char*)value);
            u32 buffer_len         = value_len + 1;
            out->value.ident.value = (char*)xl_alloc_push(alloc, buffer_len, XL_FALSE);
            if (!out->value.ident.value) {
                xl_perror("failed to allocate node identifier string buffer");
                xl_alloc_pop_to(alloc, curr_mem_pos);  // de-allocate our ast node
                return NULL;
            }
            strncpy(out->value.ident.value, (char*)value, value_len);
        }
    }

    return out;
}

#define XL_NODE_MAKE_NUMBER(alloc, value) xl_node_make((alloc), NODE_NUMBER, (XL_ANY) & (value))
#define XL_NODE_MAKE_IDENTIFIER(alloc, value) xl_node_make((alloc), NODE_IDENTIFIER, (XL_ANY)value);

inline static const char* binop_to_str(xl_binary_operand op) {
    switch (op) {
        case OP_ADD:
            return "+";
        case OP_SUBTRACT:
            return "-";
        case OP_MULTIPLY:
            return "*";
        case OP_DIVIDE:
            return "/";
        case OP_EXPONENTIATION:
            return "**";
        case OP_FLOORDIV:
            return "//";
        case OP_MODULUS:
            return "%";
        case OP_ASSIGN:
            return "=";
    }
    return "";
}

#endif
