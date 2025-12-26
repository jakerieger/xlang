#include "xcommon.h"
#include "xscanner.h"
#include "xutils.h"
#include "xalloc.h"

#define MAX_FILE_SIZE 2048

int main(int argc, char** argv) {
    if (argc < 2) {
        xl_perror("no input file provided\n");
        return 1;
    }

    // General-purpose allocator
    xl_allocator* alloc = xl_alloc_create(XL_KB(1));
    if (!alloc) {
        xl_perror("failed to create allocator");
        return 1;
    }

    char input_buffer[MAX_FILE_SIZE] = {'\0'};
    i32 read_result                  = xl_read_file(alloc, argv[1], input_buffer, MAX_FILE_SIZE);
    if (read_result != XL_OK) {
        xl_perror("failed to read source file");
        return 1;
    }

    xl_scanner* scanner = xl_scanner_create(alloc, input_buffer);
    if (!scanner) {
        xl_perror("failed to create scanner");
        return 1;
    }

    xl_token curr_token;
    assert(xl_scanner_emit(scanner, &curr_token));
    while (curr_token.type != TOKEN_EOF) {
        xl_token_print(&curr_token);
        assert(xl_scanner_emit(scanner, &curr_token));
    }

    // clean up all memory at once
    xl_alloc_destroy(alloc);

    return 0;
}
