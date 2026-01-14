// CMD: args="basic"

#include "wali_start.c"
#include <unistd.h>
#include <time.h>
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_clock_gettime")))
long __imported_wali_clock_gettime(clockid_t clock_id, struct timespec *tp);
__attribute__((__import_module__("wali"), __import_name__("SYS_clock_getres")))
long __imported_wali_clock_getres(clockid_t clock_id, struct timespec *tp);

int wali_clock_gettime(clockid_t clock_id, struct timespec *tp) { return (int)__imported_wali_clock_gettime(clock_id, tp); }
int wali_clock_getres(clockid_t clock_id, struct timespec *tp) { return (int)__imported_wali_clock_getres(clock_id, tp); }

#else
#include <sys/syscall.h>
int wali_clock_gettime(clockid_t clock_id, struct timespec *tp) { return syscall(SYS_clock_gettime, clock_id, tp); }
int wali_clock_getres(clockid_t clock_id, struct timespec *tp) { return syscall(SYS_clock_getres, clock_id, tp); }
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    struct timespec ts;
    if (wali_clock_gettime(CLOCK_MONOTONIC, &ts) != 0) return -1;
    
    if (wali_clock_getres(CLOCK_MONOTONIC, &ts) != 0) return -1;
    
    return 0;
}
