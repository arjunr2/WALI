// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <sys/types.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_getppid")))
long long __imported_wali_getppid(void);

pid_t wali_getppid(void) { return (pid_t)__imported_wali_getppid(); }

#else
#include <sys/syscall.h>
pid_t wali_getppid(void) { return syscall(SYS_getppid); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    pid_t ppid = wali_getppid();
    if (ppid <= 0) return -1; // init is 1, usually > 0
    
    return 0;
}
