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
__attribute__((__import_module__("wali"), __import_name__("SYS_clock_nanosleep")))
long __imported_wali_clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain);

int wali_clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain) {
    return (int)__imported_wali_clock_nanosleep(clock_id, flags, request, remain);
}
#else
#include <sys/syscall.h>
int wali_clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain) {
    return syscall(SYS_clock_nanosleep, clock_id, flags, request, remain);
}
#endif

int test(void) {
    if (test_init_args() != 0) return -1;
    
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000; // 1us
    
    if (wali_clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL) != 0) return -1;
    
    return 0;
}
