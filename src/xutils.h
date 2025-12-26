#ifndef X_UTILS_H
#define X_UTILS_H

#include "xcommon.h"
#include "xalloc.h"

i32 xl_read_file(xl_allocator* alloc, const char* filename, char* buffer_out, size_t buffer_size) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "error: failed to open file: %s\n", filename);
        return XL_FAIL;
    }

    fseek(fp, 0, SEEK_END);
    i32 fp_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    i32 input_size = fp_size + 1;
    if (input_size > (i32)buffer_size) {
        fprintf(stderr, "error: file larger than provided buffer (file: %d, buffer: %d)\n", fp_size, input_size);
        return XL_FAIL;
    }

    char* buffer = (char*)xl_alloc_push(alloc, input_size, XL_FALSE);
    if (!buffer) {
        perror("error: memory allocation failed");
        fclose(fp);
        return XL_FAIL;
    }

    size_t read_bytes  = fread(buffer, 1, input_size, fp);
    buffer[read_bytes] = '\0';
    fclose(fp);

    // Copy buffer contents to our output buffer
    if (!memcpy(buffer_out, buffer, input_size)) {
        perror("error: failed to copy temp buffer to output");
        return XL_FAIL;
    }

    return XL_OK;
}

#endif
