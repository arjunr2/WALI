// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_exit_group")))
long __imported_wali_exit_group(int status);


void wali_exit_group(int status) { __imported_wali_exit_group(status); }
#else
#include <sys/syscall.h>
void wali_exit_group(int status) { syscall(SYS_exit_group, status); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // This terminates the process, which is expected success for this test if return code matches.
    // The test runner checks return code.
    // We want to return 0. (Success)
    
    wali_exit_group(0);
    
    // Should not reach here
    return -1;
}
