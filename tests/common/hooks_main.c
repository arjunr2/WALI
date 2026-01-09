#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Defined in unit/<name>.c or defaulted here
__attribute__((weak)) int test_setup(int argc, char **argv) { return 0; }
__attribute__((weak)) int test_cleanup(int argc, char **argv) { return 0; }

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <setup|cleanup> [args...] [-- cmd...]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "setup") == 0) {
        // Find separator "--"
        int sep_index = -1;
        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "--") == 0) {
                sep_index = i;
                break;
            }
        }
        
        int setup_argc = 0;
        char **setup_argv = NULL;
        
        if (sep_index != -1) {
            setup_argc = sep_index - 2;
            setup_argv = &argv[2];
        } else {
            // Assume all remaining args are for setup if no command separator
            setup_argc = argc - 2;
            setup_argv = &argv[2];
        }

        int ret = test_setup(setup_argc, setup_argv);
        if (ret != 0) {
            fprintf(stderr, "test_setup failed with %d\n", ret);
            return ret;
        }
        
        // If a command is provided (after --), exec it
        if (sep_index != -1 && sep_index + 1 < argc) {
            execvp(argv[sep_index+1], &argv[sep_index+1]);
            perror("execvp setup wrapper");
            return 1;
        }
        return 0;
    } 
    else if (strcmp(argv[1], "cleanup") == 0) {
        return test_cleanup(argc - 2, &argv[2]);
    }
    
    fprintf(stderr, "Invalid mode: %s. Use 'setup' or 'cleanup'.\n", argv[1]);
    return 1;
}
