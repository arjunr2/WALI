// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/types.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_gettid")))
long long __imported_wali_gettid(void);

pid_t wali_gettid(void) { return (pid_t)__imported_wali_gettid(); }
#else
#include <sys/syscall.h>
pid_t wali_gettid(void) { return syscall(SYS_gettid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    pid_t tid = wali_gettid();
    if (tid <= 0) return -1;
    
    return 0;
}
