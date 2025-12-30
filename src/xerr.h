#ifndef X_ERR_H
#define X_ERR_H

#include "xterminal.h"

#include <stdlib.h>

/*
 * EXIT CODES FOR COMMON ERRORS
 */
#define XL_ERR_ALLOCATION_FAILED 101
#define XL_ERR_OVER_CAPACITY 102
#define XL_ERR_EXEC_COMPILE 103
#define XL_ERR_EXEC_RUNTIME 104
#define XL_ERR_OPEN_FILE 105
#define XL_ERR_EXPECTED_EXPRESSION 106

inline static char* xl_exit_code_to_str(int code) {
    switch (code) {
        case XL_ERR_ALLOCATION_FAILED:
            return "XL_ERR_ALLOCATION_FAILED";
        case XL_ERR_OVER_CAPACITY:
            return "XL_ERR_OVER_CAPACITY";
        case XL_ERR_EXEC_COMPILE:
            return "XL_ERR_EXEC_COMPILE";
        case XL_ERR_EXEC_RUNTIME:
            return "XL_ERR_EXEC_RUNTIME";
        case XL_ERR_OPEN_FILE:
            return "XL_ERR_OPEN_FILE";
        case XL_ERR_EXPECTED_EXPRESSION:
            return "XL_ERR_EXPECTED_EXPRESSION";
        default:
            return "UNKNOWN_ERROR";
    }
}

inline static void xl_panic(int code, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf(COLOR_BOLD COLOR_RED "(panicked) " COLOR_RESET);
    vprintf(fmt, args);
    printf("\nexited with code %d (%s)\n", code, xl_exit_code_to_str(code));
    va_end(args);
    exit(code);
}

inline static void xl_runtime_error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf(COLOR_BOLD COLOR_RED "(runtime error) " COLOR_RESET);
    vprintf(fmt, args);
    printf("\n");
    va_end(args);
}

#endif
