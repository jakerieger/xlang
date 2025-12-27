#include "xast.h"
#include "xcommon.h"
#include "xscanner.h"
#include "xutils.h"
#include "xalloc.h"

#define MAX_FILE_SIZE 2048

// #define ENABLE_TEST_CODE 1

static int test_code(xl_allocator* alloc) {
    xl_ast_node* node = xl_node_make_val(alloc, NODE_STRING, (char*)"Hello, World");
    if (node) {
        printf("Node: ");
        xl_node_print(node);
    }

    xl_alloc_destroy(alloc);

    return XL_OK;
}

int main(int argc, char** argv) {
#ifndef ENABLE_TEST_CODE
    if (argc < 2) {
        xl_perror("no input file provided\n");
        return XL_FAIL;
    }
#else
    XL_UNUSED(argc);
#endif

    // General-purpose allocator
    xl_allocator* alloc = xl_alloc_create(XL_KB(4));
    if (!alloc) {
        xl_perror("failed to create allocator");
        return XL_FAIL;
    }

#ifdef ENABLE_TEST_CODE
    return test_code(alloc);
#endif

    char input_buffer[MAX_FILE_SIZE] = {'\0'};
    i32 read_result                  = xl_read_file(alloc, argv[1], input_buffer, MAX_FILE_SIZE);
    if (read_result != XL_OK) {
        xl_perror("failed to read source file");
        return XL_FAIL;
    }

    xl_scanner* scanner = xl_scanner_create(alloc, input_buffer);
    if (!scanner) {
        xl_perror("failed to create scanner");
        return XL_FAIL;
    }

    xl_token curr_token;
    assert(xl_scanner_emit(scanner, &curr_token));
    while (curr_token.type != TOKEN_EOF) {
        xl_token_print(&curr_token);
        assert(xl_scanner_emit(scanner, &curr_token));
    }

    // clean up all memory at once
    xl_alloc_destroy(alloc);

    return XL_OK;
}
