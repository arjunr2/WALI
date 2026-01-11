#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "wasm_export.h"

/****************************************************************************
 * env module
 ****************************************************************************/

// 1. The actual logic
static int wali_test_setup(wasm_exec_env_t exec_env, uint32_t offset, uint32_t len) {
    wasm_module_inst_t module_inst = wasm_runtime_get_module_inst(exec_env);
    uint8_t *buffer = (uint8_t *)wasm_runtime_addr_app_to_native(module_inst, offset);
    
    if (offset % 65536 != 0) {
        printf("[Host] Error: Buffer is NOT page-aligned (offset: 0x%X).\n", offset);
        return -1;
    }
    
    const char *filepath = getenv("WALI_TEST_RESULT_FILE");
    if (!filepath) {
      printf("[Host] Error: WALI_TEST_RESULT_FILE not set\n");
      return -1;
    }

    int fd = open(filepath, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
       perror("[Host] open");
       return -1;
    }
    if (ftruncate(fd, len) != 0) {
       perror("[Host] ftruncate");
       close(fd);
       return -1;
    }

    // MAP_FIXED to replace the backing memory of the Wasm buffer with this file
    void *res = mmap(buffer, len, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0);
    close(fd);

    if (res == MAP_FAILED) {
        perror("[Host] mmap");
        return -1;
    }

    return 0;
}

// 2. Map the Wasm function name to the C function
static NativeSymbol wali_test_native_symbols[] = {
    {
        "wali_test_setup", // Name in Wasm
        wali_test_setup,   // C function pointer
        "(ii)i",           // Signature: (int, int) -> int
        NULL
    }
};

// 3. Export the library for iwasm to find
uint32_t get_native_lib(char **p_module_name, NativeSymbol **p_native_symbols) {
    *p_module_name = "env";
    *p_native_symbols = wali_test_native_symbols;
    return sizeof(wali_test_native_symbols) / sizeof(NativeSymbol);
}
