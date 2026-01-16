// CMD: setup="" args="" cleanup=""

#include "wali_start.c"

#ifdef WALI_TEST_WRAPPER
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

int test(void) {
    struct timespec res;
    
    TEST_LOG("Testing clock_getres(CLOCK_REALTIME)");
    TEST_ASSERT_EQ(wali_syscall_clock_getres(CLOCK_REALTIME, &res), 0);
    
    // Resolution should be positive
    TEST_ASSERT(res.tv_sec > 0 || (res.tv_sec == 0 && res.tv_nsec > 0));
    
    TEST_LOG("Testing clock_getres(CLOCK_MONOTONIC)");
    TEST_ASSERT_EQ(wali_syscall_clock_getres(CLOCK_MONOTONIC, &res), 0);
    
    TEST_ASSERT(res.tv_sec > 0 || (res.tv_sec == 0 && res.tv_nsec > 0));
    
    return 0;
}
