#ifndef X_PROGRAM_H
#define X_PROGRAM_H

#include "xast.h"

typedef struct {
    u64 node_count;
    xl_ast_node** nodes;
} xl_program;

#endif
