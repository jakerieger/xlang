#include "xalloc.h"
#include "xcommon.h"
#include "xutils.h"
#include "xvm.h"
#include "xversion.h"
#include <string.h>

#define MAX_FILE_SIZE XL_MB(1)
#define MAX_LINE_SIZE 1024

static void repl() {
    printf("xlang %s - Copyright (C) 2025 Jake Rieger\n", VERSION_STRING_FULL);
    printf("Type \".exit\" to quit the REPL.\n");

    const size_t buffer_size = 1024;
    char line[buffer_size];

    for (;;) {
        printf("> ");
        if (!fgets(line, buffer_size, stdin)) {
            printf("\n");
            break;
        }

        if (strcmp(line, ".exit\n") == 0) {
            break;
        }

        xl_exec_result exec_result = xl_vm_exec(line);
        if (exec_result != EXEC_OK) {
            if (exec_result == EXEC_COMPILE_ERROR) {
                xl_error("failed to compile input");
            } else if (exec_result == EXEC_RUNTIME_ERROR) {
                xl_error("an error occurred during execution");
            }
        }
    }
}

static void print_help() {
    printf("xlang %s - Copyright (C) 2025 Jake Rieger\n\n", VERSION_STRING_FULL);
    printf("USAGE\n");
    printf("  xl  -or-  xl <filename>\n\n");
    printf("ARGUMENTS\n");
    printf("  -h, --help  Show this help page\n");
    printf("\n");
}

static int execute_file(xl_allocator* alloc, const char* filename) {
    char input_buffer[MAX_FILE_SIZE] = {'\0'};
    i32 read_result                  = xl_read_file(alloc, filename, input_buffer, MAX_FILE_SIZE);
    if (read_result != XL_OK) {
        xl_alloc_destroy(alloc);
        return xl_error("failed to read source file: '%s'", filename);
    }

    xl_exec_result exec_result = xl_vm_exec(input_buffer);
    if (exec_result != EXEC_OK) {
        switch (exec_result) {
            case EXEC_COMPILE_ERROR:
                xl_alloc_destroy(alloc);
                return xl_error("failed to compile program");
            case EXEC_RUNTIME_ERROR:
                xl_alloc_destroy(alloc);
                return xl_error("an error occurred during program execution");
            default:
                break;
        }
    }

    return XL_OK;
}

int main(int argc, char* argv[]) {
    // General-purpose allocator
    xl_allocator* alloc = xl_alloc_create(XL_KB(4));
    if (!alloc) {
        return xl_error("failed to create allocator");
    }

    //****************//
    xl_vm_init(alloc);
    //****************//

    if (argc < 2) {
        repl();
        xl_alloc_destroy(alloc);
        return XL_OK;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_help();
        xl_alloc_destroy(alloc);
        return XL_OK;
    }

    int result = execute_file(alloc, argv[1]);

    // clean up all memory at once
    xl_alloc_destroy(alloc);

    return result;
}
