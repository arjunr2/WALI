#include <stdio.h>
#include "wasm_export.h"

// 1. The actual logic
static int wali_test_setup(wasm_exec_env_t exec_env, uint32_t offset, uint32_t len) {
    // Convert Wasm linear memory offset to a host pointer
    //wasm_module_inst_t module_inst = wasm_runtime_get_module_inst(exec_env);
    //uint8_t *buffer = (uint8_t *)wasm_runtime_addr_app_to_native(module_inst, offset);
    
    // Note: In the original test_stub.c, there was a check for 64k alignment.
    // We keep it here if that's the intended constraint for the test setup.
    if (offset % 65536 != 0) {
        printf("[Host] Warning: Buffer is NOT page-aligned (offset: 0x%X).\n", offset);
        return -1; // Optional: fail if strict
    }

    printf("[Host] Checking buffer at Wasm offset: 0x%X with len: %u\n", offset, len);
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
