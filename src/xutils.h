#ifndef X_UTILS_H
#define X_UTILS_H

#include "xcommon.h"
#include "xalloc.h"

inline static char* xl_read_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        xl_error("failed to open file: %s\n", filename);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    i32 fp_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    i32 input_size = fp_size + 1;
    char* buffer   = (char*)malloc(input_size);

    size_t read_bytes  = fread(buffer, 1, input_size, fp);
    buffer[read_bytes] = '\0';
    fclose(fp);

    return buffer;
}

#endif
