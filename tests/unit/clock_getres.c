// CMD: args="basic"

#include "wali_start.c"
#include <time.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

#ifdef __wasm__
__attribute__((__import_module__("wali"), __import_name__("SYS_clock_getres")))
long __imported_wali_clock_getres(clockid_t clock_id, struct timespec *res);

int wali_clock_getres(clockid_t clock_id, struct timespec *res) { 
    return (int)__imported_wali_clock_getres(clock_id, res); 
}

#else
#include <sys/syscall.h>
int wali_clock_getres(clockid_t clock_id, struct timespec *res) { 
    return syscall(SYS_clock_getres, clock_id, res); 
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    struct timespec res;
    
    // Get resolution for CLOCK_REALTIME
    if (wali_clock_getres(CLOCK_REALTIME, &res) != 0) return -1;
    
    // Resolution should be positive
    if (res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_nsec <= 0)) return -1;
    
    // Get resolution for CLOCK_MONOTONIC
    if (wali_clock_getres(CLOCK_MONOTONIC, &res) != 0) return -1;
    
    if (res.tv_sec < 0 || (res.tv_sec == 0 && res.tv_nsec <= 0)) return -1;
    
    return 0;
}
