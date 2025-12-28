#include "xalloc.h"
#include "xcommon.h"
#include "xutils.h"
#include "xvm.h"
#include "xversion.h"
#include <string.h>

#define MAX_LINE_SIZE 1024

static void repl() {
    printf(COLOR_BOLD COLOR_BRIGHT_BLUE "xlang" COLOR_RESET " %s " COLOR_DIM
                                        "- Copyright (C) 2025 Jake Rieger\n" COLOR_RESET,
           VERSION_STRING_FULL);
    printf(COLOR_ITALIC "Type " COLOR_RESET COLOR_BOLD ".exit" COLOR_RESET COLOR_ITALIC
                        " to quit the REPL.\n\n" COLOR_RESET);

    char line[MAX_LINE_SIZE];

    for (;;) {
        printf("❯ ");
        if (!fgets(line, MAX_LINE_SIZE, stdin)) {
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
    printf(COLOR_BOLD COLOR_BRIGHT_BLUE "xlang" COLOR_RESET COLOR_DIM
                                        " - Copyright (C) 2025 Jake Rieger\n" COLOR_RESET);
    printf(COLOR_DIM "version " COLOR_RESET COLOR_BOLD VERSION_STRING_FULL COLOR_RESET "\n\n");
    printf("USAGE\n");
    printf("  xl  " COLOR_DIM "-or-" COLOR_RESET "  xl <filename>\n\n");
    printf("ARGUMENTS\n");
    printf("  -h, --help  Show this help page\n");
    printf("\n");
}

static void print_version() {
    printf("version " VERSION_STRING_FULL "\n");
}

static int execute_file(const char* filename) {
    char* input = xl_read_file(filename);
    if (!input) {
        return XL_FAIL;
    }

    xl_exec_result exec_result = xl_vm_exec(input);
    free(input);

    if (exec_result != EXEC_OK) {
        switch (exec_result) {
            case EXEC_COMPILE_ERROR:
                return xl_error("failed to compile program");
            case EXEC_RUNTIME_ERROR:
                return xl_error("an error occurred during program execution");
            default:
                break;
        }
    }

    return XL_OK;
}

int main(int argc, char* argv[]) {
    xl_vm_init();

    if (argc < 2) {
        repl();
        return XL_OK;
    }

    char* input = argv[1];

    if (strcmp(input, "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_help();
        return XL_OK;
    }

    if (strcmp(input, "--version") == 0 || strcmp(input, "-v") == 0) {
        print_version();
        return XL_OK;
    }

    int result = execute_file(input);

    xl_vm_shutdown();

    return result;
}
