#include "xscanner.h"
#include "xversion.h"
#include "xvm.h"
#include "xutils.h"

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
                xl_error(XL_ERR_EXEC_COMPILE, "failed to compile input");
            } else if (exec_result == EXEC_RUNTIME_ERROR) {
                xl_error(XL_ERR_EXEC_RUNTIME, "an error occurred during execution");
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
    printf("version  : " VERSION_STRING "\n");
    printf("platform : " PLATFORM_STRING "\n");
}

static void print_vm_config(const xl_vm_config* config) {
    size_t p_scaled;
    size_t g_scaled;
    size_t t_scaled;
    size_t stack_scaled;

    const char* p_oom     = xl_bytes_order_of_magnitude(config->mem_size_permanent, &p_scaled);
    const char* g_oom     = xl_bytes_order_of_magnitude(config->mem_size_generation, &g_scaled);
    const char* t_oom     = xl_bytes_order_of_magnitude(config->mem_size_temporary, &t_scaled);
    const char* stack_oom = xl_bytes_order_of_magnitude(config->stack_size, &stack_scaled);

    printf("=== VM Configuration ===\n");
    printf("Memory (Perm) : %lu %s\n", p_scaled, p_oom);
    printf("Memory (Gen)  : %lu %s\n", g_scaled, g_oom);
    printf("Memory (Temp) : %lu %s\n", t_scaled, t_oom);
    printf("Stack Size    : %lu %s\n", stack_scaled, stack_oom);
}

static int execute_file(const char* filename) {
    char* input = xl_read_file(filename);
    if (!input) {
        return XL_FAIL;
    }

    xl_exec_result exec_result = xl_vm_exec(input);
    free(input);

    if (exec_result != EXEC_OK) {
        if (exec_result == EXEC_COMPILE_ERROR) {
            xl_error(XL_ERR_EXEC_COMPILE, "failed to compile input");
        } else if (exec_result == EXEC_RUNTIME_ERROR) {
            xl_error(XL_ERR_EXEC_RUNTIME, "an error occurred during execution");
        }
    }

    return XL_OK;
}

int main(int argc, char* argv[]) {
    xl_vm_config config;
    config.mem_size_permanent  = XL_MB(64);
    config.mem_size_generation = XL_MB(64);
    config.mem_size_temporary  = XL_MB(4);
    config.stack_size          = XL_KB(1);

    xl_vm_init(config);

    if (argc < 2) {
        repl();
        return XL_OK;
    }

    char* input_file = argv[1];

    if (strcmp(input_file, "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        print_help();
        return XL_OK;
    }

    if (strcmp(input_file, "--version") == 0 || strcmp(input_file, "-v") == 0) {
        print_version();
        return XL_OK;
    }

    if (strcmp(input_file, "--vm-config") == 0) {
        print_vm_config(&config);
        return XL_OK;
    }

    // int result = execute_file(input);

    char* source = xl_read_file(input_file);
    xl_scanner_init(source);
    xl_token curr = xl_scanner_emit();
    while (curr.type != TOKEN_EOF && curr.type != TOKEN_ERROR) {
        xl_token_print(&curr);
        curr = xl_scanner_emit();
    }

    xl_vm_shutdown();

    return XL_OK;
}
