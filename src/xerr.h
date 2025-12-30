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

inline static void xl_error(int code, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    printf(COLOR_BOLD COLOR_RED "error: " COLOR_RESET COLOR_RED "\nexited with code: %d\n", code);
    vprintf(fmt, args);
    printf(COLOR_RESET "\n");
    va_end(args);

    exit(code);
}

#endif
