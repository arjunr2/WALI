/*
 * RUN: args="hello" env="TEST_VAR=123"
 */
#include "wali_test.h"
#include <stdlib.h>

int wali_test_main(int argc, char** argv) {
    wali_test_log("Test Started");

    // Log arguments (skip argv[0] as it differs between native and wasm)
    for (int i = 1; i < argc; i++) {
        wali_test_write("Arg: ", 5);
        wali_test_log(argv[i]);
    }

    // Log environment variable
    const char* val = getenv("TEST_VAR");
    if (val) {
        wali_test_write("Env TEST_VAR: ", 14);
        wali_test_log(val);
    } else {
        wali_test_log("Env TEST_VAR not found");
    }

    wali_test_log("Test Finished");
    return 0;
}
