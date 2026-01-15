// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getpid")))
long __imported_wali_getpid(void);

int wali_getpid(void) { return (int)__imported_wali_getpid(); }

#else
#include <sys/syscall.h>
int wali_getpid(void) { return syscall(SYS_getpid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    int pid = wali_getpid();
    if (pid <= 0) return -1;
    
    // Call again to ensure consistency
    int pid2 = wali_getpid();
    if (pid != pid2) return -1;
    
    return 0;
}
