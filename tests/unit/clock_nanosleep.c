// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

int test(void) {
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 1000; // 1us
    
    TEST_LOG("Testing clock_nanosleep(CLOCK_MONOTONIC, 1us)");
    TEST_ASSERT_EQ(wali_syscall_clock_nanosleep(CLOCK_MONOTONIC, 0, &ts, NULL), 0);
    
    return 0;
}
