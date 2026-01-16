// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
#include <errno.h>

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

int test(void) {
    struct timespec ts;
    
    TEST_LOG("Testing clock_gettime(CLOCK_MONOTONIC)");
    TEST_ASSERT_EQ(wali_syscall_clock_gettime(CLOCK_MONOTONIC, &ts), 0);
    
    // Check if it looks valid (non-zero or plausible)
    TEST_ASSERT(ts.tv_sec > 0 || (ts.tv_sec == 0 && ts.tv_nsec >= 0));
    
    TEST_LOG("Testing clock_getres(CLOCK_MONOTONIC)");
    TEST_ASSERT_EQ(wali_syscall_clock_getres(CLOCK_MONOTONIC, &ts), 0);
    
    TEST_ASSERT(ts.tv_sec >= 0 && ts.tv_nsec > 0);
    
    return 0;
}
