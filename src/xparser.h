#ifndef X_PARSER_H
#define X_PARSER_H

#include "xast.h"
#include "xscanner.h"
#include "xprogram.h"

typedef struct {
    xl_allocator* alloc;
    xl_scanner* scanner;
    xl_token current_token;
} xl_expr_parser;

xl_expr_parser* xl_parser_create(xl_allocator* alloc, xl_scanner* scanner);

xl_program xl_parse_program(xl_expr_parser* parser);
xl_ast_node* xl_parse_expression(xl_expr_parser* parser);
xl_ast_node* xl_parse_term(xl_expr_parser* parser);
xl_ast_node* xl_parse_factor(xl_expr_parser* parser);
xl_ast_node* xl_parse_additive(xl_expr_parser* parser);
xl_ast_node* xl_parse_assignment(xl_expr_parser* parser);
xl_ast_node* xl_parse_func_def(xl_expr_parser* parser);
xl_ast_node* xl_parse_func_call(xl_expr_parser* parser);

#endif
