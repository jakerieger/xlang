#ifndef X_UTILS_H
#define X_UTILS_H

#include "xcommon.h"
#include "xalloc.h"

#include <math.h>
#include <float.h>

/// @brief Reads a given file to a string buffer
inline static char* xl_read_file(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        xl_panic(XL_ERR_OPEN_FILE, "failed to open file: %s\n", filename);
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

/// @brief Returns the corresponding order of magnitude suffix for the given
/// byte size (Kilobytes, Megabytes, Gigabytes, etc.) and the scaled
/// value to match
inline static char* xl_bytes_order_of_magnitude(size_t size, size_t* scaled_out) {
    if (size < XL_KB(1)) {
        *scaled_out = size;
        return "bytes";
    }

    else if (size < XL_MB(1)) {
        *scaled_out = size / XL_KB(1);
        return "Kb";
    }

    else if (size < XL_GB(1)) {
        *scaled_out = size / XL_MB(1);
        return "Mb";
    }

    else if (XL_UNLIKELY(size < ((u64)size << 40))) {
        *scaled_out = size / XL_GB(1);
        return "Gb";
    }

    return NULL;
}

/// @brief Simple string hash algorithm
inline static u32 xl_hash_string(const char* key, i32 length) {
    const u32 MN_1 = 2166136261u;
    const u32 MN_2 = 16777619;

    u32 hash = MN_1;
    for (i32 i = 0; i < length; i++) {
        hash ^= (u8)key[i];
        hash *= MN_2;
    }

    return hash;
}

#endif
