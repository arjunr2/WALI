// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/resource.h>
#include <string.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getrusage")))
long __imported_wali_getrusage(int who, struct rusage *usage);

int wali_getrusage(int who, struct rusage *usage) { return (int)__imported_wali_getrusage(who, usage); }

#else
#include <sys/syscall.h>
int wali_getrusage(int who, struct rusage *usage) { return syscall(SYS_getrusage, who, usage); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    // getrusage
    struct rusage usage;
    memset(&usage, 0, sizeof(usage));
    
    if (wali_getrusage(RUSAGE_SELF, &usage) != 0) return -1;
    
    // Basic sanity: stime or utime might be 0 if fast, but call succeeded.
    
    return 0;
}
