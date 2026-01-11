// CMD: args="a1 a2"
// CMD: args="shorthand1 shorthand2"

#include "wali_start.c"
#include <stdio.h>
#include <string.h>

int test(void) {
    if (get_args() != 0) {
        printf("Failed to get args\n");
        return -1;
    }

    printf("[Test] argc=%d\n", argc);
    for(int i=0; i<argc; i++) {
        printf("[Test] argv[%d]=%s\n", i, argv[i]);
    }
    
    // We expect certain args based on the CMDs above
    // Return 0 if we see expected args, -1 otherwise
    if (argc >= 2) {
        if (strcmp(argv[1], "a1") == 0 || strcmp(argv[1], "shorthand1") == 0) {
             return 0;
        }
    }
    
    return -1;
}
