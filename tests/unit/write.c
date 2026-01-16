// CMD: setup="" args="" cleanup=""

#include "wali_start.c"
#include <string.h>

#ifdef WALI_TEST_WRAPPER
// No setup/cleanup needed for pure pipe test
int test_setup(int argc, char **argv) { return 0; }
int test_cleanup(int argc, char **argv) { return 0; }
#endif

int test(void) {
    int pfd[2];
    TEST_ASSERT_EQ(wali_syscall_pipe2(pfd, 0), 0);

    const char *msg = "WriteTest";
    size_t len = strlen(msg);

    // Test 1: Write success
    TEST_ASSERT_EQ(wali_syscall_write(pfd[1], msg, len), (long)len);

    // Verify Read
    char buf[32];
    TEST_ASSERT_EQ(wali_syscall_read(pfd[0], buf, len), (long)len);
    TEST_ASSERT_EQ(strncmp(buf, msg, len), 0);

    // Test 2: Write 0 bytes (Success)
    TEST_ASSERT_EQ(wali_syscall_write(pfd[1], msg, 0), 0);

    // Close pipe ends
    wali_syscall_close(pfd[0]);
    wali_syscall_close(pfd[1]);

    // Test 3: Write to closed FD (Fail)
    long res = wali_syscall_write(pfd[1], msg, len);
    TEST_ASSERT(res < 0);
    // TEST_ASSERT_EQ(res, -1);

    return 0;
}
