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
        return 1;
    }
    
    // Check expected values
    if (strcmp(val, "hello") != 0 && strcmp(val, "world") != 0) {
        return 1;
    }

    return 0;
}
