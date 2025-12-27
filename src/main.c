#include "xscanner.h"
#include "xutils.h"
#include "xparser.h"

#define MAX_FILE_SIZE 2048

int main(int argc, char** argv) {
    if (argc < 2) {
        xl_perror("no input file provided\n");
        return XL_FAIL;
    }

    // General-purpose allocator
    xl_allocator* alloc = xl_alloc_create(XL_KB(4));
    if (!alloc) {
        xl_perror("failed to create allocator");
        return XL_FAIL;
    }

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

    xl_expr_parser* parser = xl_parser_create(alloc, scanner);
    if (!parser) {
        xl_perror("failed to create expression parser");
        return XL_FAIL;
    }

    xl_program program = xl_parse_program(parser);

    // clean up all memory at once
    xl_alloc_destroy(alloc);

    return XL_OK;
}
