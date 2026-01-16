// CMD: env="TEST_VAR=hello"
// CMD: env="TEST_VAR=world"

#include "wali_start.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int test(void) {
    test_init_env();

    char *val = getenv("TEST_VAR");
    if (!val) {
        TEST_FAIL("TEST_VAR not found");
    }
    
    // Check expected values
    if (strcmp(val, "hello") != 0 && strcmp(val, "world") != 0) {
        TEST_FAIL("Unexpected TEST_VAR value");
    }

    return 0;
}

