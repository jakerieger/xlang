#include "xalloc.h"
#include "xutils.h"
#include "xvm.h"

#define MAX_FILE_SIZE 2048

int main(int argc, char** argv) {
    if (argc < 2) {
        return xl_error("no input file provided\n");
    }

    // General-purpose allocator
    xl_allocator* alloc = xl_alloc_create(XL_KB(4));
    if (!alloc) {
        return xl_error("failed to create allocator");
    }

    char input_buffer[MAX_FILE_SIZE] = {'\0'};
    i32 read_result                  = xl_read_file(alloc, argv[1], input_buffer, MAX_FILE_SIZE);
    if (read_result != XL_OK) {
        xl_alloc_destroy(alloc);
        return xl_error("failed to read source file: '%s'", argv[1]);
    }

    //*[MAKE SURE WE CALL THIS]*//
    xl_vm_init(alloc);
    //**************************//

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

    // clean up all memory at once
    xl_alloc_destroy(alloc);

    return XL_OK;
}
